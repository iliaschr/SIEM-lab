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
- RAM: `4 GB`
- DISK: `60 GB`
- CPU Cores: `2`
- Notes: TBA
- Problems faced: Windows cannot read the ProductKey [solution](https://www.reddit.com/r/virtualbox/comments/1c1o605/error_installing_windows_windows_cannot_read_the/)
