# BluGate
### Goal:
<ul>
<li> Opening door/gate with Bluetooth or Bluetooth Low Energy (BLE) connection without any app (queen, ideal case)</li>
<li> Opening door/gate with Bluetooth or BLE </li>
<li> Opening door/gate by MAC address with BLE devices such as smart bands, smart watches working with BLE protocol</li>
<li> Different modes</li>
<li> Admin mode for changing password, changing modes, adding other ble devices by its MAC address</li>
</ul>

### Esp32 as a Client (Central or Scanner)
<ul>
<li> Can scan and add mac addresses which can connect (mi band, other devices can open)</li>
<li> get RSSI is simple </li>
<li> ios 5+ is supported?</li>
<li> android 6+ supported? (some android 5 is supported depending on chip)</li>
</ul>


### Esp32 as a Server (Peripheral or Advertiser)
<ul>
<li> Older device can connect</li>
<li> Multiconnect (quantity?)</li>
<li> Sending rssi from phone, and disconnect if needed? (library should change sligthly to disconnect phone)</li>
<li> hard to get RSSI (didn't know yet how)</li>
<li> android 4.33+ supported</li>
<li> ios 5+ is supported</li>
</ul>

### Web Bluetooth
<ul>
<li> Access from some of the browsers (Chrome, Opera)</li>
<li> Some of the Android devices supporter</li>
<li> IOS is not supported</li>
</ul>



### Modes:
Ranges can be set (RSSI)
1.	Tap. Bringing your phone close to the door, just like tapping (current range is 10 cm)
2.	Touch. Have more range of 1-1.5 meters, touch sensor can be set
3.	Far. Maximum range of BLE, about 10 meters.
4.	Free Touch. Open to everyone if touch sensor is touched. (not realized yet, however manageable task) 



### What is BLE? https://centare.com/blog/2019/03/what_is_bluetooth_low_energy/
 ![image](https://user-images.githubusercontent.com/26284578/62289733-456c0a00-b481-11e9-8272-7c54b28adca0.png)
 
 
UUID – unique id used for identification of Services and Characteristics
Scheme below can be useful when creating mobile app.

![image](https://user-images.githubusercontent.com/26284578/62289829-93810d80-b481-11e9-80eb-188eadb08ab8.png)

### How BlueGate works? 
<p>BlueGate scans for devices which has service with UUID “ba10” and connects to this device. Next, it tries to connect to the characteristics “ba30” if there is no such characteristics it disconnects from the connected device (or can be restarted). Each characteristic has values which can be read and written. BlueGate reads value of “ba30” and check if it matches with password. If password is correct door will be opened, otherwise it will not be opened. After checking password, it will disconnect from device and ESP will be restarted to avoid RAM memory problems.</p> 
<p>Each BlueGate device has serial password (constant) which can give access to the Admin mode. If “ba30” value matches with serial password, it opens Admin mode, more specifically it will try to access and read values of “ba40” and “ba50”. Password will be changed to value of “ba40” if it differs from current password. Value of “ba50” sets the mode of the BlueGate. If “ba30” value.</p></br>
Current password is 123456, mode is “tap”, serial password is 060593
<table style="width:100%">
  <tr>
    <th></th>
    <th>ba30 value</th> 
    <th>ba40 value</th>
    <th>ba50 value</th>
    <th>Result</th>
  </tr>
  <tr>
    <td>User</td>
    <td>123456</td>
    <td>Don't care</td>
    <td>Don't care</td>
    <td>Door opens</td>
  </tr>
  <tr>
    <td>User</td>
    <td>111111</td>
    <td>Don't care</td>
    <td>Don't care</td>
    <td>Door doesn't open</td>
  </tr>
  <tr>
    <td>Adding MAC</td>
    <td>mac</td>
    <td>Don't care</td>
    <td>Don't care</td>
    <td>In the next loop, adds device close to BlueGate</td>
  </tr>
  <tr>
    <td>Admin</td>
    <td>060593</td>
    <td>654321</td>
    <td>tap</td>
    <td>Password changes to 654321</td>
  </tr>
  <tr>
    <td>Admin</td>
    <td>060593</td>
    <td>123456</td>
    <td>touch</td>
    <td>Mode changes to touch</td>
  </tr>
</table>

### BlueGate Flash System
Preferences:</br>
Preferences provides persistent (across resets) but mutable storage of various types of variables. It is similar to EEPROM library in Arduino, but more simple.</br> 
SPIFFS is a file system that can manage flash memory of a device.
<table style="width:100%">
  <tr>
    <th>Name</th>
    <th>File System</th> 
    <th>Data type</th>
    <th>Purpose</th>
  </tr>
  <tr>
    <td>password</td>
    <td>Preferences</td>
    <td>String</td>
    <td>Storing password</td>
  </tr>
  <tr>
    <td>mode</td>
    <td>Preferences</td>
    <td>String</td>
    <td>Storing mode</td>
  </tr>
  <tr>
    <td>addMode</td>
    <td>Preferences</td>
    <td>bool</td>
    <td>Adding mac address of a device</td>
  </tr>
  <tr>
    <td>mac.txt</td>
    <td>SPIFFS</td>
    <td>text</td>
    <td>Storing mac addresses of devices</td>
  </tr>
	
</table>


Each time ESP32 restarts it reads all necessary information from the flash memory.
 
### Used libraries
All used libraries are in folder, while doing project I used Platformio IDE.
<table style="width:100%">
  <tr>
    <th>Library</th>
    <th>Use</th> 
  </tr>
  <tr>
    <td>ESP32_BLE_Arduino</td>
    <td>BLE, slightly differs from original library</td>
  </tr>
  <tr>
    <td>SPIFFS</td>
    <td>File system manager of flash memory</td>
  </tr>	
  <tr>
    <td>Preferences</td>
    <td>Similar to EEPROM, used to store variables</td>
  </tr>
  <tr>
    <td>Wire</td>
    <td>Library to use I2C to display</td>
  </tr>
  <tr>
    <td>SSD1306Wire</td>
    <td>OLED display library</td>
  </tr>
 </table>
	

### OLED Display Examples
![image](https://user-images.githubusercontent.com/26284578/62290473-430aaf80-b483-11e9-91ce-3954faae56eb.png)


