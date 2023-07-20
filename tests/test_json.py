import pytest
import subprocess



json_file = "file:/xrootd/tests/storage.json?volume={}&protocol={}"
test_data =[
    ("/store/temp/test","TEST_JSON","root","/store/test"),
    ("/store/temp/test","TEST_JSON","prefix","root://cern-vm//cms/store/temp/test"),
    ("/test/mc/file0","CMSSST_TEST","root","root://host.domain/aaa/bbb//test/mc/file0"),
    ("/other/mc/file1","CMSSST_TEST","root","root://host.domain/aaa/bbb//store/nomc/file1/trailer"),
    ("/other/mc/file2","CMSSST_TEST","root","root://host.domain/aaa/bbb//store/nomc/file2/trailer"),
    ("store/mc/file3","CMSSST_TEST","root","root://host.domain/aaa/bbb/never/first/xmc/file3/trailer"),
    ("other/mc/file4","CMSSST_TEST","root","root://host.domain/aaa/bbb/other/xmc/file4"),
    ("/store/data/file5","CMSSST_TEST","root","root://host.domain/aaa/bbb//store/first/data/file5/trailer")
]

@pytest.mark.parametrize("lfn,volume,protocol,pfn", test_data)
def test_json(lfn,volume,protocol,pfn):
    output = subprocess.check_output([
        "./build/test.out", lfn, json_file.format(volume,protocol)],encoding='utf-8')

    assert pfn == output.strip()