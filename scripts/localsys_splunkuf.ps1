# Stop the service first
Stop-Service -Name SplunkForwarder -Force

# Change the service account using WMI
$service = Get-WmiObject -Class Win32_Service -Filter "Name='SplunkForwarder'"
$result = $service.Change($null,$null,$null,$null,$null,$null,"LocalSystem","")

# Check if the change was successful
if ($result.ReturnValue -eq 0) {
    Write-Host "Service account changed successfully to LocalSystem"
} else {
    Write-Host "Failed to change service account. Return code: $($result.ReturnValue)"
}

# Start the service
Start-Service -Name SplunkForwarder

# Verify the change
Get-WmiObject Win32_Service -Filter "Name='SplunkForwarder'" | Select-Object Name, StartName