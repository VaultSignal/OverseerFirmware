from fastapi import FastAPI, WebSocket

app = FastAPI()


"""From basic tutorial at https://fastapi.tiangolo.com/advanced/websockets/"""
@app.websocket('/')
async def point(websocket: WebSocket):
    await websocket.accept()
    while True:
        data = await websocket.receive_json()
        print(data)