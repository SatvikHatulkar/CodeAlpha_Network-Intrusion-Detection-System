#!/bin/bash

apt install snort -y
mv /etc/snort/rules/local.rules /etc/snort/rules/old_local.rules
cp local.rules /etc/snort/rules/local.rules
cp block_local.rules /etc/snort/rules/block_local.rules
mv /etc/snort/snort.lua /etc/snort/old_snort.lua
cp snort.lua /etc/snort/snort.lua

echo ""
echo "Setup completed successfully!"
echo ""

echo "Useful Commands and Instructions:"
echo "--------------------------------------"
echo ""
echo "Check Snort version:"
echo "   snort -V"
echo ""
echo "Start Snort in IDS mode:"
echo "   snort -c /etc/snort/snort.lua -R /etc/snort/rules/local.rules -i <interface> -A alert_fast"
echo ""
echo "Start Snort in Inline IPS mode:"
echo "   (Make sure to use block_local.rules instead of local.rules)"
echo "   snort -c /etc/snort/snort.lua -Q --daq afpacket --daq-mode inline -R /etc/snort/rules/block_local.rules -i <interface> -A alert_fast"
echo ""
echo "--------------------------------------"
echo ""

echo "Available Network Interfaces on this system:"
ip -o link show | awk -F': ' '{print "   "$2}'

echo ""
echo "All done! Replace <interface> with your actual network interface name from the list above."
echo ""
