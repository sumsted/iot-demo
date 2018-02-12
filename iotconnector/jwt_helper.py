import datetime
import jwt


class JwtHelper:

    def __init__(self, project_id, private_key_file, algorithm):
        self.project_id = project_id
        self.private_key_file = private_key_file
        self.algorithm = algorithm
        self.jwt, self.expiration = JwtHelper.create_jwt(project_id, private_key_file, algorithm)

    def valid_token(self):
        return True if datetime.datetime.utcnow() >= self.expiration else False

    def get_valid_jwt(self):
        if self.valid_token():
            return self.jwt
        else:
            self.jwt, self.expiration = JwtHelper.create_jwt(self.project_id, self.private_key_file, self.algorithm)
            return self.jwt

    @staticmethod
    def create_jwt(project_id, private_key_file, algorithm):
        now = datetime.datetime.utcnow()
        expiration = now + datetime.timedelta(minutes=60)
        token = {
                # The time that the token was issued at
                'iat': now,
                # The time the token expires.
                'exp': expiration,
                # The audience field should always be set to the GCP project id.
                'aud': project_id
        }

        # Read the private key file.
        with open(private_key_file, 'r') as f:
            private_key = f.read()

        print('Creating JWT using {} from private key file {}'.format(
                algorithm, private_key_file))

        return jwt.encode(token, private_key, algorithm=algorithm), expiration