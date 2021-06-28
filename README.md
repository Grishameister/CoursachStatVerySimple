# very_very_simple_server

Простой сервер с отдачей статики, можно отрефакторить код, конечно.

# Запуск сервера:
```
sudo docker build -t highload_server . && sudo docker run -p 8889:8889 highload_server
```
# Результаты функционального тестирования:
```
./httptest.py localhost 8889
test_directory_index (__main__.HttpServer)
directory index file exists ... ok
test_document_root_escaping (__main__.HttpServer)
document connection escaping forbidden ... ok
test_empty_request (__main__.HttpServer)
Send empty line ... ok
test_file_in_nested_folders (__main__.HttpServer)
file located in nested folders ... ok
test_file_not_found (__main__.HttpServer)
absent file returns 404 ... ok
test_file_type_css (__main__.HttpServer)
Content-Type for .css ... ok
test_file_type_gif (__main__.HttpServer)
Content-Type for .gif ... ok
test_file_type_html (__main__.HttpServer)
Content-Type for .html ... ok
test_file_type_jpeg (__main__.HttpServer)
Content-Type for .jpeg ... ok
test_file_type_jpg (__main__.HttpServer)
Content-Type for .jpg ... ok
test_file_type_js (__main__.HttpServer)
Content-Type for .js ... ok
test_file_type_png (__main__.HttpServer)
Content-Type for .png ... ok
test_file_type_swf (__main__.HttpServer)
Content-Type for .swf ... ok
test_file_urlencoded (__main__.HttpServer)
urlencoded filename ... ok
test_file_with_dot_in_name (__main__.HttpServer)
file with two dots in name ... ok
test_file_with_query_string (__main__.HttpServer)
query string with get params ... ok
test_file_with_slash_after_filename (__main__.HttpServer)
slash after filename ... ok
test_file_with_spaces (__main__.HttpServer)
filename with spaces ... ok
test_head_method (__main__.HttpServer)
head method support ... ok
test_index_not_found (__main__.HttpServer)
directory index file absent ... ok
test_large_file (__main__.HttpServer)
large file downloaded correctly ... ok
test_post_method (__main__.HttpServer)
post method forbidden ... ok
test_request_without_two_newlines (__main__.HttpServer)
Send GET without to newlines ... ok
test_server_header (__main__.HttpServer)
Server header exists ... ok

----------------------------------------------------------------------
Ran 24 tests in 0.042s

OK

```

# Benchmark highload_server(in docker)

```
ab -c 200 -n 30000 http://localhost:8889/httptest/wikipedia_russia.html
This is ApacheBench, Version 2.3 <$Revision: 1807734 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 3000 requests
Completed 6000 requests
Completed 9000 requests
Completed 12000 requests
Completed 15000 requests
Completed 18000 requests
Completed 21000 requests
Completed 24000 requests
Completed 27000 requests
Completed 30000 requests
Finished 30000 requests


Server Software:        highload_server
Server Hostname:        localhost
Server Port:            8889

Document Path:          /httptest/wikipedia_russia.html
Document Length:        954824 bytes

Concurrency Level:      200
Time taken for tests:   31.029 seconds
Complete requests:      30000
Failed requests:        0
Total transferred:      28647510000 bytes
HTML transferred:       28644720000 bytes
Requests per second:    966.84 [#/sec] (mean)
Time per request:       206.860 [ms] (mean)
Time per request:       1.034 [ms] (mean, across all concurrent requests)
Transfer rate:          901611.11 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    3   1.2      3      18
Processing:     7  203  17.8    202     313
Waiting:        1    6  10.1      4     139
Total:         24  207  17.6    206     316

Percentage of the requests served within a certain time (ms)
  50%    206
  66%    208
  75%    211
  80%    212
  90%    217
  95%    224
  98%    246
  99%    274
 100%    316 (longest request)

```

# Benchmark nginx(without docker and sendfile on)
```
ab -c 200 -n 30000 http://localhost:8080/httptest/wikipedia_russia.html
This is ApacheBench, Version 2.3 <$Revision: 1807734 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 3000 requests
Completed 6000 requests
Completed 9000 requests
Completed 12000 requests
Completed 15000 requests
Completed 18000 requests
Completed 21000 requests
Completed 24000 requests
Completed 27000 requests
Completed 30000 requests
Finished 30000 requests


Server Software:        nginx/1.14.0
Server Hostname:        localhost
Server Port:            8080

Document Path:          /httptest/wikipedia_russia.html
Document Length:        954824 bytes

Concurrency Level:      200
Time taken for tests:   26.338 seconds
Complete requests:      30000
Failed requests:        0
Total transferred:      28652130000 bytes
HTML transferred:       28644720000 bytes
Requests per second:    1139.03 [#/sec] (mean)
Time per request:       175.588 [ms] (mean)
Time per request:       0.878 [ms] (mean, across all concurrent requests)
Transfer rate:          1062356.48 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    3   0.9      3      11
Processing:     1  172  12.3    172     338
Waiting:        0    4   8.4      3     151
Total:          1  175  12.4    175     340

Percentage of the requests served within a certain time (ms)
  50%    175
  66%    177
  75%    180
  80%    181
  90%    185
  95%    188
  98%    193
  99%    222
 100%    340 (longest request)

```
