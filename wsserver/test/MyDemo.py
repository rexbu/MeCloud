s = 'u="2|1:0|10:1503571258|1:u|32:NTk5YTU0YWQ1MTE1OWE3MTlmZTI3OTM0|2ca9a2bc0963e5e378d0297bdcee23a702937ed1f2c7a2e802780e9118d33e4b; expires=Sat, 23 Sep 2017 10:40:58 GMT; Path=/'

r1 = s.split('=')[1]
r2 = r1.split(';')[0]
print(r2.replace('"',''))