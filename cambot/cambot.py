import telepot
import time
import picamera

import cv2

import os

class FabCamera:
  def __init__(self):
      self.token = os.environ['FABCAMBOT_TOKEN']

  def take_image(self):
      #cam = cv2.VideoCapture(0)
      #ret_val, img = cam.read()
      #cv2.imshow('cam',img)
      self.last_img = "./imgs/" + time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime()) + ".jpg"
      #picam.capture(self.last_img, resize=(320,240))
      picam.capture(self.last_img)
  
  def handle(self,msg):
    
      chat_id = msg['chat']['id']
      user_name = msg['from']['first_name']
      cmd = msg['text']
      
      txt = 'Got %s from %s' % (cmd,user_name)
      print(txt)
      bot.sendMessage(chat_id=chat_id, text=txt)
      
      commands = ('/captura','/foto','/captura@fabcambot','/foto@fabcambot')
      for available_cmd in commands:
          if cmd == available_cmd:
              bot.sendMessage(chat_id=chat_id, text='Sending photo..')
              self.take_image()
              photo = open(self.last_img,'rb')
              bot.sendPhoto(chat_id=chat_id,photo=photo)

picam = picamera.PiCamera()    
cam = FabCamera()

#print(cam.token)

bot = telepot.Bot(cam.token)
bot.message_loop(cam.handle)
print ('I am reading the messages')

while 1:
    time.sleep(10)
