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
- OS: Ubuntu Server 22.04 LTS
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
