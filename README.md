BigSeg
======

Chinese Word Segmentation 中文分词

Usage:
`bigseg.exe termlistfile FW_Idxfile inputfile outputfile stepsize`

Example:
`bigseg.exe  term_dict.txt  index1.txt   test_cop.txt  index1_out.txt   300`

term_dict.txt

    狝猴桃颗粒
    獴大口非鲫
    獴属
    珮夫人止咳露
    珺宝
    珺芙蓉
    瑇玳
    瑯玡榆
    甴曱草
    痟肾
    痟瘦
    痟中
    瘈疭
    瘈脉
    瘜肉
    癒创木基
    癓瘕
    癥积
    啊朴啡
    啊糖腺苷
    阿吽
    阿·沃号轮案
    阿埃二氏
    阿埃二氏病变
    阿埃二氏手术

test_cop.txt

    獴属珮夫人止咳露很管用珺宝阿·沃号轮案是吧阿埃二氏的阿埃二氏病变，阿埃二氏手术！啊糖腺苷非常消瘦，獴大口非鲫大口吃狝猴桃颗粒
    
index1.txt

    狝|||0
    獴|||2
    珮|||3
    珺|||5
    瑇|||6
    瑯|||7
    甴|||8
    痟|||11
    瘈|||13
    瘜|||14
    癒|||15
    癓|||16
    癥|||17
    啊|||19
    阿|||24

index1_out.txt

    1###獴属|||珮夫人止咳露|||珺宝|||阿·沃号轮案|||阿埃二氏|||阿埃二氏病变|||阿埃二氏手术|||啊糖腺苷|||獴大口非鲫|||
