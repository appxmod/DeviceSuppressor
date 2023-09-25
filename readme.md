Enable, disable or print device by hid / class / desc.


## disable by class name 

```
DeviceSuppressor.exe disablebyclass="mouse"
```


### disable devices whose description is ends with "mouse"

the first `?` indicates a wildcard match:

```
DeviceSuppressor.exe disablebydesc="?*mouse"
```

### disable by hardware device id



```
DeviceSuppressor.exe disablebyid="USB\VID_000"
```

or simpely:


```
DeviceSuppressor.exe disable="USB\VID_000"
```

to enable : 

```
DeviceSuppressor.exe enable="USB\VID_000"
```


### Print


print ids:
```
DeviceSuppressor.exe print
```

print class names:
```
DeviceSuppressor.exe print=byclass
```
