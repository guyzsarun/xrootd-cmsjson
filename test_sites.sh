#!/usr/bin/bash
protocol_json='XRootD'
protocol_xml="eos"

for site in $(ls /root/SITECONF | grep "^T.*");
do
    echo "------------------------" >> test.txt
    echo -e "$site" >> test.txt
    echo "------------------------" >> test.txt

    echo -e "#### JSON ####" >> test.txt
    if grep --quiet $protocol_json /root/SITECONF/$site/storage.json; then
        for volume in $(cat "/root/SITECONF/$site/storage.json" | grep volume | cut -d '"' -f 4);
        do
            echo -e "+ $volume" >> test.txt
            ~/output/json.out "file:/root/SITECONF/$site/storage.json?volume=$volume&protocol=$protocol_json" >> test.txt
            echo -e "\n"
        done
    fi

    echo -e "#### TFC ####" >> test.txt
    if grep --quiet $protocol_xml /root/SITECONF/$site/PhEDEx/storage.xml; then
        ~/output/test.out "file:/root/SITECONF/$site/PhEDEx/storage.xml?protocol=$protocol_xml" >> test.txt
    fi

done

