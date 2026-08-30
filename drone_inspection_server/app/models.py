from sqlalchemy import Column, Integer, DateTime, Float, String, Enum, ForeignKey
from .database import Base, engine

class Mission(Base):
    __tablename__ = 'mission'
    mission_id = Column(Integer, primary_key=True)
    start_time = Column(DateTime)
    end_time = Column(DateTime)

class  Area(Base):
    __tablename__ = 'area'
    area_id = Column(Integer, primary_key=True)
    x = Column(Float)
    y = Column(Float)
    z = Column(Float)
    yaw = Column(Float)
    pitch = Column(Float)
    bolt_num = Column(Integer)

class Inspection(Base):
    __tablename__ = 'inspection'
    inspection_id = Column(Integer, primary_key=True)
    area_id = Column(Integer, ForeignKey('area.area_id'))
    mission_id = Column(Integer, ForeignKey('mission.mission_id'))
    detected_bolts = Column(Integer)
    result = Column(Enum('OK', 'NG', 'LOOSE'))
    image_path = Column(String)
    capture_timestamp = Column(DateTime)
    capture_x = Column(Float)
    capture_y = Column(Float)
    capture_z = Column(Float)

Base.metadata.create_all(engine)