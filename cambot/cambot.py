import telepot
import time
import picamera

import cv2
import random
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
      print('Image %s taken :)' % (self.last_img))
  
  def handle(self,msg):
    
      self.chat_id = msg['chat']['id']
      self.user_name = msg['from']['first_name']
      self.cmd = msg['text']
      
      txt = self.rand_txt()
      
      print(txt)
      bot.sendMessage(chat_id=self.chat_id, text=txt)
      
      commands = ('/captura','/foto','/captura@fabcambot','/foto@fabcambot')
      for available_cmd in commands:
          if self.cmd == available_cmd:
              bot.sendMessage(chat_id=self.chat_id, text='Sending photo..')
              self.take_image()
              photo = open(self.last_img,'rb')
              bot.sendPhoto(chat_id=self.chat_id,photo=photo)

  def rand_txt(self):
      list = ['Got %s from %s' % (self.cmd,self.user_name),'%s says %s'%(self.user_name,self.cmd),
		'did you say %s?'%(self.cmd)]
      return random.choice(list)

picam = picamera.PiCamera()    
cam = FabCamera()
picam.vflip = True
picam.hflip = True

#print(cam.token)

bot = telepot.Bot(cam.token)
bot.message_loop(cam.handle)
print ('I am reading the messages')

while 1:
    time.sleep(1)
