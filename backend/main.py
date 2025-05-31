from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import ctypes
import os

app = FastAPI()
from fastapi.middleware.cors import CORSMiddleware

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"], 
    allow_methods=["*"],
    allow_headers=["*"],
)


LIB_PATH = os.path.join(os.path.dirname(__file__), "../cpp_backend/librestaurant.so")
if os.name == "nt":
    LIB_PATH = os.path.join(os.path.dirname(__file__), "../cpp_backend/restaurant.dll")

lib = ctypes.CDLL(LIB_PATH)

BUFFER_SIZE = 65536

lib.get_all_restaurants.argtypes = [ctypes.c_char_p, ctypes.c_size_t]
lib.get_all_restaurants.restype = None

lib.filter_by_type.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t]
lib.filter_by_type.restype = None

lib.filter_by_cost.argtypes = [ctypes.c_double, ctypes.c_char_p, ctypes.c_size_t]
lib.filter_by_cost.restype = None

lib.search_by_name.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_size_t]
lib.search_by_name.restype = None

lib.get_nearby.argtypes = [ctypes.c_char_p, ctypes.c_double, ctypes.c_char_p, ctypes.c_size_t]
lib.get_nearby.restype = None

class TypeRequest(BaseModel):
    type: str

class CostRequest(BaseModel):
    max_cost: float

class NameRequest(BaseModel):
    name: str

class NearbyRequest(BaseModel):
    location: str
    max_km: float

@app.get("/restaurants")
def get_all_restaurants():
    buf = ctypes.create_string_buffer(BUFFER_SIZE)
    lib.get_all_restaurants(buf, BUFFER_SIZE)
    return {"restaurants": buf.value.decode("utf-8")}

@app.post("/restaurants/filter/type")
def filter_type(req: TypeRequest):
    buf = ctypes.create_string_buffer(BUFFER_SIZE)
    lib.filter_by_type(req.type.encode('utf-8'), buf, BUFFER_SIZE)
    return {"filtered": buf.value.decode("utf-8")}

@app.post("/restaurants/filter/cost")
def filter_cost(req: CostRequest):
    buf = ctypes.create_string_buffer(BUFFER_SIZE)
    lib.filter_by_cost(req.max_cost, buf, BUFFER_SIZE)
    return {"filtered": buf.value.decode("utf-8")}

@app.post("/restaurants/search")
def search_name(req: NameRequest):
    buf = ctypes.create_string_buffer(BUFFER_SIZE)
    lib.search_by_name(req.name.encode('utf-8'), buf, BUFFER_SIZE)
    return {"result": buf.value.decode("utf-8")}

@app.post("/restaurants/nearby")
def nearby(req: NearbyRequest):
    buf = ctypes.create_string_buffer(BUFFER_SIZE)
    lib.get_nearby(req.location.encode('utf-8'), req.max_km, buf, BUFFER_SIZE)
    return {"nearby": buf.value.decode("utf-8")}
