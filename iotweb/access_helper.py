import hashlib
import uuid

from settings import Settings

settings = Settings()


def client_key_hash():
    key = str(uuid.uuid4())
    hash_hex = hash_it(key, settings.WEB['ACCESS'])
    return key, hash_hex


def hash_it(k1):
    big_key = (k1+settings.WEB['ACCESS']).encode()
    hash_result = hashlib.sha256(big_key)
    hash_hex = hash_result.hexdigest()
    return hash_hex


def valid_hash(k1, h2):
    test_hash = hash_it(k1)
    return h2 == test_hash


if __name__ == '__main__':
    client_key = str(uuid.uuid4())
    client_hash = hash_it(client_key)
    print(client_key, client_hash, valid_hash(client_key, client_hash))
    client_key = str(uuid.uuid4())
    print(client_key, client_hash, valid_hash(client_key, client_hash))
