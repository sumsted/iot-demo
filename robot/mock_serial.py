

class Serial:
    def __init__(self, p1, p2, timeout):
        pass

    def readline(self):
        return str.encode("{\"status\":\"success\"}\n")

    def write(self, param):
        return len(param)