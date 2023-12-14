from kivy.app import App
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.gridlayout import GridLayout
from kivy.properties import NumericProperty, ObjectProperty, StringProperty
import csv
import pandas as pd
import numpy as np
import time
from kivy.clock import Clock
from kivy.core.window import Window
from kivy.uix.image import Image
import time
import pyautogui

Window.fullscreen = 'auto'
Window.maximize =True
#Window.size = (1380,480)
Window.borderless = True


class LeftLayout(GridLayout):

    num1 = NumericProperty(150)
    num2 = NumericProperty(150)
    num3 = NumericProperty(150)
    num4 = NumericProperty(150)
    val1 = StringProperty('--')
    val2 = StringProperty('--')
    val3 = StringProperty('--')
    val4 = StringProperty('--')
    unit1 = StringProperty('%')
    unit2 = StringProperty(':20')
    unit3 = StringProperty('°C')
    unit4 = StringProperty('A')
    val11 = float(150)
    val22 = float(150)
    val33 = float(150)
    val44 = float(150)
    ksekarfwma = StringProperty('--')
    color1=.14,.8,.9,.9
    color3=.14,.8,.9,.9


    def show_data(self, filename):
        data = pd.read_csv(filename, error_bad_lines = False)
        data1 = np.array(data.iloc[-1].values)
        self.ksekarfwma = data1[0]
        for x in range(-1, -15, -1):
            if str((np.array(data.iloc[x].values))[20]) == " ":
                self.val1 = str((np.array(data.iloc[-1].values)[20])) # state of charge (from BMS)
                self.val2 = str((np.array(data.iloc[-1].values)[22])) # autonomy (from BMS)
                self.val3 = str((np.array(data.iloc[-1].values)[21])) # battery temperature (from BMS)

            else:
                self.val1 = str(np.array(data.iloc[x].values)[20]) # state of charge (from BMS)
                self.val2 = str(np.array(data.iloc[x].values)[22]) # autonomy (from BMS)
                self.val3 = str(np.array(data.iloc[x].values)[21]) # battery temperature (from BMS)
                self.num1 = self.fit_to_scale(self.val11, 0, 100)
                self.num3 = self.fit_to_scale(self.val33, 0, 50)
                try:
                    self.val11 = round(float((np.array(data.iloc[x].values)[20]))) # state of charge (from BMS | circle)
                    self.val33 = round(float((np.array(data.iloc[x].values)[21]))) # battery temperature (from BMS | line)
                except:
                    pass
                break

        for x in range(-1, -15, -1):
            if str((np.array(data.iloc[x].values))[18]) == " ":
                self.val4 = str(data1[18])                                         # battery current (from BMS)
            else:
                self.val4 = str((np.array(data.iloc[x].values)[18]))
                self.num4 = self.fit_to_scale(self.val44, 0, 400)
                try:
                    self.val44 = round(float((np.array(data.iloc[x].values)[18])))
                except:
                    pass
                break

        '''
        self.ksekarfwma = data1[0]
        self.val1 = str(data1[20])
        #self.val2 = str(data1[3]) autonomy
        self.val3 = str(data1[21])
        self.val4 = str(data1[14])
        self.num1 = self.fit_to_scale(self.val11, 0, 100)
        self.num3 = self.fit_to_scale2(self.val33, 0, 65)
        self.num4 = self.fit_to_scale2(self.val44, 0, 400)
        try:
            self.val11 = round(float(data1[20]))
            self.val22 = round(float(data1[3]))
            self.val33 = round(float(data1[21]))
            self.val44 = round(float(data1[14]))
        except:
            pass
            '''

    # circle
    def fit_to_scale(self, value, minimum, maximum):
        ok =  ((value - minimum)/ (maximum - minimum) * 300) - 150
        return round(ok)

    # line
    def fit_to_scale2(self, value, minimum, maximum):
        ok =  (value - minimum)/ (maximum - minimum) * 150
        return round(ok)

    def update(self, *args):

        if self.val11<18:                   # Min(state of charge)=18
            pyautogui.click(x=115,y=120)
        if self.val33>50:                   # Max(motor temperature)=65
            pyautogui.click(x=115,y=360)
        if self.val44>400:                  # Max(battery temperature)=400
            pyautogui.click(x=345,y=360)

        self.show_data('data.csv')

    def __init__(self, **kwargs):
        super(LeftLayout, self).__init__(**kwargs)
        self.cols = 2

class MiddleLayout(BoxLayout):

    num5 = NumericProperty(150)
    num6 = NumericProperty(150)
    num7 = NumericProperty(150)
    val5 = StringProperty('--')
    val6 = StringProperty('--')
    val7 = StringProperty('--')
    unit5 = StringProperty('kn')
    unit6 = StringProperty(' miles')
    unit7 = StringProperty(' miles')
    val55 = float(150)
    val66 = float(150)
    val77 = float(150)

    def show_data(self, filename):
        data = pd.read_csv(filename, error_bad_lines = False)
        data1 = np.array(data.iloc[-1].values)
        self.val5 = str(data1[3])                              # Speed(knots)
        self.val6 = str(data1[4])                              # Miles Total
        self.val7 = str(data1[5])                              # Miles per lap
        self.num5 = self.fit_to_scale(self.val55, 0, 40)
        try:
            self.val55 = round(float(data1[3]))
            self.val66 = round(float(data1[4]))
            self.val77 = round(float(data1[5]))
        except:
            pass

    def fit_to_scale(self, value, minimum, maximum):
        ok =  ((value - minimum)/ (maximum - minimum) * 300) - 150
        return round(ok)

    def update(self, *args):
        self.show_data('data.csv')

class RightLayout(GridLayout):

    num8 = NumericProperty(150)
    num9 = NumericProperty(150)
    num10 = NumericProperty(150)
    num11 = NumericProperty(150)
    val8 = StringProperty('--')
    val9 = StringProperty('--')
    val10 = StringProperty('--')
    val11 = StringProperty('--')
    unit8 = StringProperty(' rpm')
    unit9 = StringProperty('°C')
    unit10 = StringProperty('A')
    unit11 = StringProperty('°C')
    val88 = float(150)
    val99 = float(150)
    val110 = float(150)
    val121 = float(150)

    def show_data(self, filename):
        data = pd.read_csv(filename, error_bad_lines = False)
        data1 = np.array(data.iloc[-1].values)
        for x in range(-1, -15, -1):
            if str((np.array(data.iloc[x].values))[8]) == " ":
                self.val8 = str(np.array(data.iloc[-1].values)[8])    # RPM (from VESC)
                self.val9 = str(np.array(data.iloc[-1].values)[11])   # MOSFET Temperature (from VESC)
                self.val10 = str(np.array(data.iloc[-1].values)[13])  # Motor Current (from VESC)
                self.val11 = str(np.array(data.iloc[-1].values)[12])  # Motor Temperature (from VESC)
            else:
                self.val8 = str(np.array(data.iloc[x].values)[8])
                self.val9 = str(np.array(data.iloc[x].values)[11])
                self.val10 = str(np.array(data.iloc[x].values)[13])
                self.val11 = str(np.array(data.iloc[x].values)[12])
                self.num8 = self.fit_to_scale(self.val88, 0, 10000)
                self.num9 = self.fit_to_scale(self.val99, 0, 90)
                self.num10 = self.fit_to_scale(self.val110, 0, 250)
                self.num11 = self.fit_to_scale(self.val121, 0, 75)
                try:
                    self.val88 = round(float((np.array(data.iloc[x].values)[8])))
                    self.val99 = round(float((np.array(data.iloc[x].values)[11])))
                    self.val110 = round(float((np.array(data.iloc[x].values)[13])))
                    self.val121 = round(float((np.array(data.iloc[x].values)[12])))
                except:
                    pass
                break



    def fit_to_scale(self, value, minimum, maximum):
        ok =  ((value - minimum)/ (maximum - minimum) * 300) - 150
        return round(ok)

    def fit_to_scale2(self, value, minimum, maximum):
        ok =  (value - minimum)/ (maximum - minimum) * 150
        return round(ok)

    def update(self, *args):
        if self.val99>90:
            pyautogui.click(x=1495,y=120)
        if self.val110>250:
            pyautogui.click(x=1265,y=360)
        if self.val121>75:
            pyautogui.click(x=1495,y=360)
        self.show_data('data.csv')

    def __init__(self, **kwargs):
        super(RightLayout, self).__init__(**kwargs)
        self.cols = 2

class MyRootLayout(BoxLayout):
    pass

class MyTestApp(App):

    def build(self):
        return MyRootLayout()

    def on_start(self):
        Clock.schedule_interval(self.root.ids.lftl.update, 0.5)
        Clock.schedule_interval(self.root.ids.mdll.update, 0.5)
        Clock.schedule_interval(self.root.ids.rgtl.update, 0.5)


if __name__ == '__main__':

    MyTestApp().run()
