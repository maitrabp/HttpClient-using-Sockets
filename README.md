# HttpClient-using-Sockets

# Introduction
Here I've made an HTTP Client application using C++ sockets. The client is able to send HTTP requests to web servers such as http://www.google.com and more. This applciation is made in C++, which means it runs through CLI commands. 

# Commands
+ `--help`:  will display all commands provided by the application which you may use. It also gives you an example usage of each command. 
+ `--profile`: will allow you to send multiple requests to the client. Then it displays some stats on the responses received. 
  + Usage Example: `--profile 2` : Make 2 HTTP requests to the address provided. 
  + If you would like to make multiple requests, you must use `--profile X` before `--url` command.
+ `--url`: allows you to type the url to which you want to make your request to.
  + Usage Example: `--url http://www.google.com`: Makes HTTP request to the provided URL. If `--profile X` was executed prior to this command, then it makes X amount of requests.
+ `--exit`: will terminate the application.


# Issues
+ The application is not able to successfully recieve a response from secured sites such as `https://www.amazon.com`. For this to happen, I will need an SSL connection to each address with HTTPS. A 3-step handshake must be made first to connect to the server successfully, then an SSL certificate with it's keys is necessary to receive a sussessful response. THis must be done for each website with a secure security layer.

+ The process stated above is slightly complex for me to implement in C++ as a rookie, I spent roughly 2-3 days on this without success. If given the chance, I'd be happy to learn from an expert. However, within the time frame given, I was able to learn and implement a client which is functional with all non-secure websites.
