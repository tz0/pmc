# pmc.c
A simple example about using Intel performance counter (PMC) in C.  

Usage:
`sudo ./pmc [enum1] [umask1] [enum2] [umask2]`


Note:
- 2 PMCs are tested each time.
- To set the environment, root privilege is required. 
- Example below is based on a Kaby Lake machine with iso-ed core 3. Change accordingly if other msr and cores are in use.



Example:
1. enable msr \
`sudo ./enable-msr.sh`
2. build \
`make`
3. run \
`sudo ./pmc 0xe 0x1 0xa1 0x80`
4. output: \
`[result] [pmc1 pmc2] = [96 5]`


Details of PMC configurations such as events, event numbers and umasks can be found from the Intel manuals:
https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html



Submit an issue if any questions.
