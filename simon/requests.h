#ifndef REQUESTS_H
#define REQUESTS_H

#include <EEPROM.h>
#include <WiFi.h>


typedef struct {
    char serial[11];
    char deviceId[11];
    char model[11];
    char firmware[11];
    char wifiSsid[11];
    char wifiPassword[21];
    char location[51];
    char gatewayProtocol[11];
    char gatewayHost[51];
    uint16_t gatewayPort;
    char gatewayPath[51];
    char state[4][11];
    char padding[15];
} ConfigurationType;

union ConfigurationUnion{
    ConfigurationType configuration;
    unsigned char bytes[250];
};

class Requests {
    public:
        Requests();
        Requests(ConfigurationType *newConfig);

        int getRequest(char *payload);
        int postRequest(char *payload);
        void printConfig(char *label, ConfigurationUnion *config);

    private:
        ConfigurationUnion config;
        uint32_t localIp;

        void readEepromConfig();
        void writeNewConfig(ConfigurationUnion *newConfig);

        void establishWifiConnection();
        const char* rootCa = \
            "-----BEGIN CERTIFICATE-----\n" \
            "MIIHBDCCBeygAwIBAgIQDsXt9/Xufb31Y+8kkj4d8jANBgkqhkiG9w0BAQsFADBg\n" \
            "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
            "d3cuZGlnaWNlcnQuY29tMR8wHQYDVQQDExZHZW9UcnVzdCBUTFMgUlNBIENBIEcx\n" \
            "MB4XDTE4MDExNjAwMDAwMFoXDTIwMDcyODEyMDAwMFowbjELMAkGA1UEBhMCVVMx\n" \
            "ETAPBgNVBAgTCE5ldyBZb3JrMREwDwYDVQQHEwhOZXcgWW9yazEgMB4GA1UEChMX\n" \
            "QWRhZnJ1aXQgSW5kdXN0cmllcyBMTEMxFzAVBgNVBAMMDiouYWRhZnJ1aXQuY29t\n" \
            "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoDWx99MOqsP1TOrUlEmR\n" \
            "Bp+3oqTXacWHlIAVhzxJeLPS6cYTCxR+aWevcLbBpDMNrCfdDCkhVtz8Aa9vg+j+\n" \
            "lWHJ8bmZFSLX7kP6eV+Uuhq/85C0+T1cCTsGH7t6CNs/SakGVtp703GGkWyzsfUp\n" \
            "2RC1tlzp+a6B+U3eLzKvC6fYPZ5Q0n/+LTgEkwwx3fxS8O4jEeQ5lxxwcDDyRAsL\n" \
            "q7VfoZcb6E8keJZZI15UluqyXmRrkBTh4fmhaxTlJgwbjqS9qnWi9MFuvkKYl5N/\n" \
            "2xR+uTzXcKO+PhUbKoaj/JG7bO3BmKy8NfagwA1iKXz1PUgBDjiR28HhOJk9aXtX\n" \
            "BQIDAQABo4IDqjCCA6YwHwYDVR0jBBgwFoAUlE/UXYvkpOKmgP792PkA76O+Alcw\n" \
            "HQYDVR0OBBYEFHz6P/pfmX6jXVMh+W+yymuMj7SaMCcGA1UdEQQgMB6CDiouYWRh\n" \
            "ZnJ1aXQuY29tggxhZGFmcnVpdC5jb20wDgYDVR0PAQH/BAQDAgWgMB0GA1UdJQQW\n" \
            "MBQGCCsGAQUFBwMBBggrBgEFBQcDAjA/BgNVHR8EODA2MDSgMqAwhi5odHRwOi8v\n" \
            "Y2RwLmdlb3RydXN0LmNvbS9HZW9UcnVzdFRMU1JTQUNBRzEuY3JsMEwGA1UdIARF\n" \
            "MEMwNwYJYIZIAYb9bAEBMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8vd3d3LmRpZ2lj\n" \
            "ZXJ0LmNvbS9DUFMwCAYGZ4EMAQICMHYGCCsGAQUFBwEBBGowaDAmBggrBgEFBQcw\n" \
            "AYYaaHR0cDovL3N0YXR1cy5nZW90cnVzdC5jb20wPgYIKwYBBQUHMAKGMmh0dHA6\n" \
            "Ly9jYWNlcnRzLmdlb3RydXN0LmNvbS9HZW9UcnVzdFRMU1JTQUNBRzEuY3J0MAkG\n" \
            "A1UdEwQCMAAwggH4BgorBgEEAdZ5AgQCBIIB6ASCAeQB4gB3AKS5CZC0GFgUh7sT\n" \
            "osxncAo8NZgE+RvfuON3zQ7IDdwQAAABYQB8kDkAAAQDAEgwRgIhAJn4/8o6iaCR\n" \
            "9S3y+V0G8HD9Bgx6Awyz2U2GpvqpUyhfAiEAmDnTbtPsxsJfGySRt/tMr5EZXrpX\n" \
            "zvxaV0EzmPgkreoAdwCHdb/nWXz4jEOZX73zbv9WjUdWNv9KtWDBtOr/XqCDDwAA\n" \
            "AWEAfJEBAAAEAwBIMEYCIQDwMKe4IWAxLD8JgJGvhd470BXgiMMuu6L+12pthRV6\n" \
            "gwIhAPL/mvtLIKKcA4zFAmaE1w/Pby3I+R/YR1JODFeh0as4AHYAu9nfvB+KcbWT\n" \
            "lCOXqpJ7RzhXlQqrUugakJZkNo4e0YUAAAFhAHyQ7gAABAMARzBFAiEAghX/klFY\n" \
            "wCoJQwb7nToe9FuFJGWifGKZEauEDdDekaQCICfDVo0xcXHT0lVJTX+ogvSTSOUp\n" \
            "z2zULZFpX1dhmwykAHYAb1N2rDHwMRnYmQCkURX/dxUcEdkCwQApBo2yCJo32RMA\n" \
            "AAFhAHyTMAAABAMARzBFAiEAp0FlA1tHTvOnXfwaULSgSbsX6950ANzLCdSQw9Yf\n" \
            "8hcCIDIY4sJrDRD91oZNpcPET6kiOsEr/JnV3TmQ/JYXDcWYMA0GCSqGSIb3DQEB\n" \
            "CwUAA4IBAQA5rD64pRGUxfsWYYrKFL2kOpSIoiTaDDfmffSjcKJgyOwM1/BSshOq\n" \
            "eRtYb2wv3jr7tXJVRNdBeVW8G8uqOp1fQWHEwnBapETib2vdNP2tCnYzDM1ye/+1\n" \
            "d5wtw8yLZWNgrl4Q/udqNlXr+uQw5FviKy5kJ5iyPGke3nRrLs7vMUgwVW25YdBz\n" \
            "GHFLhNh2rR9RVRPwqCURizrDCBlQBBbq5BIBoCeLU22ljM34LavRb6MpLdf+Kei5\n" \
            "fzg4GAgeSytwomUWgS5he6kA/WBsrDNt5eexnX/wJ+lrK4rKxypoVsGl3mdV650t\n" \
            "rFmtB45Df5BELL2q72wsYDUNDdIQRMFM\n" \
            "-----END CERTIFICATE-----\n";
};


#endif