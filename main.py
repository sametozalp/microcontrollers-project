import cv2
from cv2 import tracker
import torch
import firebase_admin
from firebase_admin import credentials, storage, db
import threading
import time
import uuid
import serial
import time

ser = serial.Serial('COM7', 9600)

def save_url_in_firebase(download_url):
    ref = db.reference('/')

    ref.set({
        'url': download_url,
    })
    
    print("resim url'i firebase'e kaydedildi..")

    
def send_data(data):
    ser.write(str(data).encode())

    
def get_url(path):
    blob = bucket.blob(path)
    blob.make_public()
    download_url = blob.public_url
    print("resim url alındı..")
    return download_url

def upload_image(local_file_path, random_uuid):
    blob = bucket.blob("iot/" + str(random_uuid) + ".png")
    blob.upload_from_filename(local_file_path)
    blob.make_public()
    print("resim yüklendi.")
    
###################################################################################

def start_upload_thread():
    while 1:
        if frame_ is not None:
            cv2.imwrite('a.png', frame_)
            random_uuid = uuid.uuid4()
            upload_image("a.png", random_uuid)
            download_url = get_url("iot/" + str(random_uuid) + ".png")
            save_url_in_firebase(download_url)
            time.sleep(10)

###################################################################################

cred = credentials.Certificate("storage.json")
firebase_admin.initialize_app(cred, {
        'storageBucket': 'gs://uploadimageiot.appspot.com/',
        'databaseURL': 'https://uploadimageiot-default-rtdb.firebaseio.com/'
    })

bucket = storage.bucket('uploadimageiot.appspot.com')

thread = threading.Thread(target=start_upload_thread)
a = 1

############################################################################################

model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)
cap = cv2.VideoCapture(0)
t = tracker.Tracker()

while True:
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    frame = cv2.resize(frame,(1920,1080))
    results = model(frame) # görüntüyü modelle işler
    
    list = [] # koordinat listesi

    for index, row in results.pandas().xyxy[0].iterrows():
        x1 = int(row['xmin'])
        y1 = int(row['ymin'])
        x2 = int(row['xmax'])
        y2 = int(row['ymax'])
        b = str(row['name']) # algılanan nesnenin adını alır

        if 'person' in b:
             list.append([x1,y1,x2,y2])
             
    person_list = t.update(list)
    
    frame_ = frame

    for box_id in person_list:
        x,y,w,h,id = box_id
        cv2.rectangle(frame_, (x,y), (w,h), (255,0,255), 2)
    
    print("Kişi Sayisi: ", len(person_list))  
    cv2.putText(frame_, "Kisi Sayisi: " + str(len(person_list)), (20,60), cv2.FONT_HERSHEY_PLAIN, 3, (0,0,255), 3)
    cv2.imshow('Mikrodenetleyiciler', frame)

    send_data(len(person_list))
    
    if a == 1:
        #thread.start()
        a = 0
        
    if cv2.waitKey(1) & 0xFF==27:
        break

cap.release()
cv2.destroyAllWindows()