import time
import pyaudio
import traceback
import numpy as np
from datetime import datetime
from collections import deque
from ctypes import c_short, cdll
from multiprocessing import freeze_support
from multiprocessing.pool import ThreadPool as Pool


def record(buffer_queue):
    print('Start recording')
    while stream.is_active():
        try:
            data = stream.read(CHUNK)
            chunk_data = np.frombuffer(data, dtype=np.int16)
            buffer_queue.append(chunk_data)
        except Exception as e:
            print(e)
            traceback.print_exc()


def listen(kws, buffer_queue):
    while True:
        try:
            time.sleep(0.01)
            if len(buffer_queue) > 0:
                raw_input_buffer = buffer_queue[0]
                kws_input_buffer = kws_test(*raw_input_buffer)
                is_wakeup = kws.py_wakeup(kws_input_buffer, len(raw_input_buffer))
                if is_wakeup > 0:
                    print('Last wakeup:', is_wakeup, ' - ', datetime.now())
                buffer_queue.popleft()
        except Exception as e:
            print(e)
            traceback.print_exc()


if __name__ == '__main__':
    CHUNK = 1024
    SAMPLE_RATE = 16_000
    CHANNELS = 1

    kws_ = cdll.LoadLibrary('../cmake-build-debug/libckws.dylib')
    kws_.py_init(None)
    kws_test = (c_short * CHUNK)

    recorder = pyaudio.PyAudio()
    stream = recorder.open(format=pyaudio.paInt16,
                           channels=CHANNELS,
                           rate=SAMPLE_RATE,
                           input=True,
                           frames_per_buffer=CHUNK)

    try:
        freeze_support()
        pool = Pool(2)
        buffer_queue_ = deque(maxlen=10)

        # start record
        pool.apply_async(record, (buffer_queue_,))
        pool.apply_async(listen, (kws_, buffer_queue_,))

        pool.close()
        pool.join()
    except KeyboardInterrupt:
        stream.stop_stream()
        stream.close()
        recorder.terminate()
        kws_.py_close()
