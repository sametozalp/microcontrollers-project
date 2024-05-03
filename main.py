import cv2
import torch
from tracker import *

model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)
cap=cv2.VideoCapture(1)
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
    for box_id in kisi_sayisi:
        x,y,w,h,id=box_id
        cv2.rectangle(frame, (x,y), (w,h), (255,0,255), 2)
    
    print("Kişi Sayisi: ", len(kisi_sayisi))  
    cv2.putText(frame, "Kisi Sayisi: " + str(len(kisi_sayisi)), (20,60), cv2.FONT_HERSHEY_PLAIN, 3, (0,0,255), 3)
    cv2.imshow('Mikrodenetleyiciler',frame)
    if cv2.waitKey(1) & 0xFF==27:
        break

cap.release()
cv2.destroyAllWindows()