import cv2
import os
import time
from PIL import Image
from tensorflow.keras.preprocessing.image import img_to_array
from tensorflow.keras.models import load_model
from tensorflow.keras.applications.mobilenet_v2 import preprocess_input
import numpy as np
import cvlib #
faceCascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
model = load_model("opencv\mask_recog_ver2.h5")
#cascPath_face = "opencv\haarcascade_frontalface_default.xml"
#faceCascade = cv2.CascadeClassifier(cascPath_face)
#cascPath_mouth = "opencv\haarcascade_mcs_mouth.xml"
#mouthCascade = cv2.CascadeClassifier(cascPath_mouth)
#cascPath_nose = "opencv\haarcascade_mcs_nose.xml"
img_list = ['opencv\img001.jpg', 'opencv\img002.jpg', 'opencv\img003.png', 'opencv\img004.png', 'opencv\img005.jpg', 'opencv\img006.jpg', 'opencv\img007.jpeg', 'opencv\img008.jpg', 'opencv\img009.jpg', 'opencv\img010.png']

for i in range(10):
    img = cv2.imread(img_list[i])
    #img_resize = cv2.resize(img,dsize=(300,300),interpolation=cv2.INTER_AREA)
    gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    #faces=faceCascade.detectMultiScale( gray, scaleFactor=1.055,minNeighbors=13, minSize=(60, 60), flags=cv2.CASCADE_SCALE_IMAGE )
    faces,confidences = cvlib.detect_face(img)
    #mouth=mouthCascade.detectMultiScale(gray, 1.3, 3)
    faces_list=[]
    preds=[]
    for (x,y,w,h) in faces:
        face_frame = img[y:h,x:w]
        face_frame = cv2.cvtColor(face_frame, cv2.COLOR_BGR2RGB)
        face_frame = cv2.resize(face_frame, (224, 224))
        face_frame = img_to_array(face_frame)
        face_frame = np.expand_dims(face_frame, axis=0)
        face_frame =  preprocess_input(face_frame)
        faces_list.append(face_frame)
        if len(faces_list)>0:
            preds = model.predict(faces_list)
        for pred in preds:
            (mask, withoutMask) = pred
        label = "Mask" if mask > withoutMask else "No Mask"
        color = (0, 255, 0) if label == "Mask" else (0, 0, 255)
        label = "{}: {:.2f}%".format(label, max(mask, withoutMask) * 100)
        cv2.putText(img, label, (x, y- 10),cv2.FONT_HERSHEY_SIMPLEX, 0.45, color, 2)
        cv2.rectangle(img, (x, y), (w, h),color, 2)
    cv2.imwrite(f'opencv/test{i}.png',img)
    cv2.imshow('faces',img)
    cv2.waitKey(0)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
