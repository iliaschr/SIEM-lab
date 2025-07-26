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
I also downloaded `Sysmon` and `Winlogbeat`.

### Attacker-Kali

Just made sure all the things I need were there.

## Creating Host-Only Network in VirtualBox

Click `File` → `Tool` → `Network Manager`. Now, click create and leave DHCP disabled.

### Assign Host-Only Adapter to Each VM

Open VM Settings and go to `Network`.
The settings should look like this: (I will place screenshots later)
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

### Set Static IP on Windows 10 VM

Here we can just open the `Control Panel` > `Network and Sharing Center`. Click on `Ethernet` and `Properties`. Select `Internet Protocol Version 4 (TCP/IPv4)` > Click `Properties`.
Set:
- IP: 192.168.56.20
- Subnet: 255.255.255.0
- Gateway: 192.168.56.1
- DNS: 8.8.8.8
(I will add a screenshot here later)

To check if it works just do: `ping 192.168.56.10`
(I will add a screenshot here later example_ping.png)
