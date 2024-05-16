import cv2
import torch
from tracker import *
import firebase_admin
from firebase_admin import credentials, storage, db
import threading
import time
import uuid

def save_url_in_firebase(download_url):
    ref = db.reference('/')

    ref.set({
        'url': download_url,
    })
    
    print("resim url'i firebase'e kaydedildi..")
    
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
cap=cv2.VideoCapture(0)
tracker = Tracker()

while True:
    ret,frame = cap.read()
    frame = cv2.flip(frame, 1)
    frame = cv2.resize(frame,(1920,1080))
    results = model(frame)
    list=[]
    for index,row in results.pandas().xyxy[0].iterrows():
        x1 = int(row['xmin'])
        y1 = int(row['ymin'])
        x2 = int(row['xmax'])
        y2 = int(row['ymax'])
        b = str(row['name'])
        if 'person' in b:
             list.append([x1,y1,x2,y2])
             
    kisi_sayisi = tracker.update(list)
    
    frame_ = frame

    for box_id in kisi_sayisi:
        x,y,w,h,id=box_id
        cv2.rectangle(frame_, (x,y), (w,h), (255,0,255), 2)
    
    print("Kişi Sayisi: ", len(kisi_sayisi))  
    cv2.putText(frame_, "Kisi Sayisi: " + str(len(kisi_sayisi)), (20,60), cv2.FONT_HERSHEY_PLAIN, 3, (0,0,255), 3)
    cv2.imshow('Mikrodenetleyiciler', frame)
    
    if a == 1:
        thread.start()
        a = 0
        
    if cv2.waitKey(1) & 0xFF==27:
        break

cap.release()
cv2.destroyAllWindows()