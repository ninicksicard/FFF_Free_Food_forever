import math


class Angle:
    MODES = ["RADIAN", "DEGREE"]
    DEFAULT = 0
    
    def __init__(self, value: float, mode: int) -> None:
        if mode == 0 :
            self._value = float(value/(2*math.pi))
        else :
            self._value = float(value/360)
    
    
    @property
    def rad(self) -> float:
        return self._value*2*math.pi
    
    @property
    def deg(self) -> float:
        return self._value*360
    
    @property
    def hom(self) -> float:
        return self._value
    
    @property
    def homogeneous(self):
        return self.hom
    
    def set(self, value, mode=DEFAULT):
        if mode :
            self._value = float(value/360)
            print("set from degree", value," to ", self._value)
        else :
            self._Vaue = self._value = float(value/(2*math.pi))
            
            
    def __repr__(self):
        if self.DEFAULT == 0:
            return self.rad
        else:
            return self.deg