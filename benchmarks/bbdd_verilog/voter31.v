// Benchmark "top" written by ABC on Fri Feb  7 13:44:00 2014

module top ( 
    pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09, pi10, pi11,
    pi12, pi13, pi14, pi15, pi16, pi17, pi18, pi19, pi20, pi21, pi22, pi23,
    pi24, pi25, pi26, pi27, pi28, pi29, pi30,
    po0  );
  input  pi00, pi01, pi02, pi03, pi04, pi05, pi06, pi07, pi08, pi09,
    pi10, pi11, pi12, pi13, pi14, pi15, pi16, pi17, pi18, pi19, pi20, pi21,
    pi22, pi23, pi24, pi25, pi26, pi27, pi28, pi29, pi30;
  output po0;
  wire n32, n34, n35, n36, n37, n38, n39, n40, n41, n42, n43, n44, n45, n46,
    n47, n48, n49, n50, n51, n52, n53, n54, n55, n56, n57, n58, n59, n60,
    n61, n62, n63, n64, n65, n66, n67, n68, n69, n70, n71, n72, n73, n74,
    n75, n76, n77, n78, n79, n80, n81, n82, n83, n84, n85, n86, n87, n88,
    n89, n90, n91, n92, n93, n94, n95, n96, n97, n98, n99, n100, n101,
    n102, n103, n104, n105, n106, n107, n108, n109, n110, n111, n112, n113,
    n114, n115, n116, n117, n118, n119, n120, n121, n122, n123, n124, n125,
    n126, n127, n128, n129, n130, n131, n132, n133, n134, n135, n136, n137,
    n138, n139, n140, n141, n142, n143, n144, n145, n146, n147, n148, n149,
    n150, n151, n152, n153, n154, n155, n156, n157, n158, n159, n160, n161,
    n162, n163, n164, n165, n166, n167, n168, n169, n170, n171, n172, n173,
    n174, n175, n176, n177, n178, n179, n180, n181, n182, n183, n184, n185,
    n186, n187, n188, n189, n190, n191, n192, n193, n194, n195, n196, n197,
    n198, n199, n200, n201, n202, n203, n204, n205, n206, n207, n208, n209,
    n210, n211, n212, n213, n214, n215, n216, n217, n218, n219, n220, n221,
    n222, n223, n224, n225, n226, n227, n228, n229, n230, n231, n232, n233,
    n234, n235, n236, n237, n238, n239, n240, n241, n242, n243, n244, n245,
    n246, n247, n248, n249, n250, n251, n252, n253, n254, n255, n256, n257,
    n258, n259, n260, n261, n262, n263, n264, n265, n266, n267, n268, n269,
    n270, n271, n272, n273, n274, n275, n276, n277, n278, n279, n280, n281,
    n282, n283, n284, n285, n286, n287, n288, n289, n290, n291, n292, n293,
    n294, n295, n296, n297, n298, n299, n300, n301, n302, n303, n304, n305,
    n306, n307, n308, n309, n310, n311, n312, n313, n314, n315, n316, n317,
    n318, n319, n320, n321, n322, n323, n324, n325, n326, n327, n328, n329,
    n330, n331, n332, n333, n334, n335, n336, n337, n338, n339, n340, n341,
    n342, n343, n344, n345, n346, n347, n348, n349, n350, n351, n352, n353,
    n354, n355, n356, n357, n358, n359, n360, n361, n362, n363, n364, n365,
    n366, n367, n368, n369, n370, n371, n372, n373, n374, n375, n376, n377,
    n378, n379, n380, n381, n382, n383, n384, n385, n386, n387, n388, n389,
    n390, n391, n392, n393, n394, n395, n396, n397, n398;
  bufx g000(.A(pi00), .O(n32));
  bufx g001(.A(n34), .O(po0));
  orx  g002(.A(n43), .B(n35), .O(n34));
  orx  g003(.A(n42), .B(n36), .O(n35));
  andx g004(.A(n41), .B(n37), .O(n36));
  andx g005(.A(n38), .B(n108), .O(n37));
  andx g006(.A(n40), .B(n39), .O(n38));
  orx  g007(.A(n64), .B(n68), .O(n39));
  orx  g008(.A(n71), .B(n53), .O(n40));
  andx g009(.A(pi30), .B(n82), .O(n41));
  andx g010(.A(n297), .B(n214), .O(n42));
  orx  g011(.A(n45), .B(n44), .O(n43));
  andx g012(.A(n213), .B(n109), .O(n44));
  andx g013(.A(n108), .B(n46), .O(n45));
  orx  g014(.A(n48), .B(n47), .O(n46));
  andx g015(.A(n96), .B(n83), .O(n47));
  andx g016(.A(n82), .B(n49), .O(n48));
  orx  g017(.A(n51), .B(n50), .O(n49));
  andx g018(.A(n63), .B(n53), .O(n50));
  andx g019(.A(n71), .B(n52), .O(n51));
  orx  g020(.A(n63), .B(n53), .O(n52));
  orx  g021(.A(n59), .B(n54), .O(n53));
  andx g022(.A(n56), .B(n55), .O(n54));
  invx g023(.A(n135), .O(n55));
  orx  g024(.A(n58), .B(n57), .O(n56));
  andx g025(.A(n116), .B(n126), .O(n57));
  andx g026(.A(n62), .B(n125), .O(n58));
  andx g027(.A(n135), .B(n60), .O(n59));
  orx  g028(.A(n61), .B(n113), .O(n60));
  andx g029(.A(n62), .B(n126), .O(n61));
  invx g030(.A(n116), .O(n62));
  andx g031(.A(n68), .B(n64), .O(n63));
  orx  g032(.A(n66), .B(n65), .O(n64));
  andx g033(.A(n231), .B(n235), .O(n65));
  andx g034(.A(n234), .B(n67), .O(n66));
  invx g035(.A(n231), .O(n67));
  orx  g036(.A(n70), .B(n69), .O(n68));
  andx g037(.A(n128), .B(n131), .O(n69));
  andx g038(.A(n132), .B(n127), .O(n70));
  orx  g039(.A(n77), .B(n72), .O(n71));
  andx g040(.A(n74), .B(n73), .O(n72));
  invx g041(.A(n239), .O(n73));
  orx  g042(.A(n76), .B(n75), .O(n74));
  andx g043(.A(n221), .B(n80), .O(n75));
  andx g044(.A(n81), .B(n230), .O(n76));
  andx g045(.A(n239), .B(n78), .O(n77));
  orx  g046(.A(n79), .B(n218), .O(n78));
  andx g047(.A(n81), .B(n80), .O(n79));
  invx g048(.A(n230), .O(n80));
  invx g049(.A(n221), .O(n81));
  orx  g050(.A(n96), .B(n83), .O(n82));
  invx g051(.A(n84), .O(n83));
  orx  g052(.A(n90), .B(n85), .O(n84));
  andx g053(.A(n89), .B(n86), .O(n85));
  andx g054(.A(n88), .B(n87), .O(n86));
  orx  g055(.A(n93), .B(n145), .O(n87));
  orx  g056(.A(n95), .B(n179), .O(n88));
  invx g057(.A(n112), .O(n89));
  andx g058(.A(n91), .B(n112), .O(n90));
  orx  g059(.A(n94), .B(n92), .O(n91));
  andx g060(.A(n93), .B(n145), .O(n92));
  invx g061(.A(n179), .O(n93));
  andx g062(.A(n179), .B(n95), .O(n94));
  invx g063(.A(n145), .O(n95));
  invx g064(.A(n97), .O(n96));
  orx  g065(.A(n103), .B(n98), .O(n97));
  andx g066(.A(n102), .B(n99), .O(n98));
  andx g067(.A(n101), .B(n100), .O(n99));
  orx  g068(.A(n285), .B(n250), .O(n100));
  orx  g069(.A(n107), .B(n284), .O(n101));
  invx g070(.A(n217), .O(n102));
  andx g071(.A(n104), .B(n217), .O(n103));
  orx  g072(.A(n106), .B(n105), .O(n104));
  andx g073(.A(n285), .B(n250), .O(n105));
  andx g074(.A(n284), .B(n107), .O(n106));
  invx g075(.A(n250), .O(n107));
  orx  g076(.A(n213), .B(n109), .O(n108));
  orx  g077(.A(n111), .B(n110), .O(n109));
  andx g078(.A(n179), .B(n145), .O(n110));
  andx g079(.A(n144), .B(n112), .O(n111));
  orx  g080(.A(n114), .B(n113), .O(n112));
  andx g081(.A(n116), .B(n125), .O(n113));
  andx g082(.A(n135), .B(n115), .O(n114));
  orx  g083(.A(n125), .B(n116), .O(n115));
  orx  g084(.A(n121), .B(n117), .O(n116));
  andx g085(.A(n120), .B(n118), .O(n117));
  andx g086(.A(n119), .B(n186), .O(n118));
  orx  g087(.A(n124), .B(n192), .O(n119));
  invx g088(.A(n182), .O(n120));
  andx g089(.A(n122), .B(n182), .O(n121));
  orx  g090(.A(n123), .B(n180), .O(n122));
  andx g091(.A(n124), .B(n192), .O(n123));
  invx g092(.A(n187), .O(n124));
  invx g093(.A(n126), .O(n125));
  orx  g094(.A(n131), .B(n127), .O(n126));
  invx g095(.A(n128), .O(n127));
  orx  g096(.A(n130), .B(n129), .O(n128));
  andx g097(.A(n194), .B(n203), .O(n129));
  andx g098(.A(n204), .B(n193), .O(n130));
  invx g099(.A(n132), .O(n131));
  orx  g100(.A(n134), .B(n133), .O(n132));
  andx g101(.A(n160), .B(n169), .O(n133));
  andx g102(.A(n170), .B(n159), .O(n134));
  orx  g103(.A(n140), .B(n136), .O(n135));
  andx g104(.A(n139), .B(n137), .O(n136));
  andx g105(.A(n138), .B(n152), .O(n137));
  orx  g106(.A(n143), .B(n158), .O(n138));
  invx g107(.A(n148), .O(n139));
  andx g108(.A(n141), .B(n148), .O(n140));
  orx  g109(.A(n142), .B(n146), .O(n141));
  andx g110(.A(n143), .B(n158), .O(n142));
  invx g111(.A(n153), .O(n143));
  orx  g112(.A(n179), .B(n145), .O(n144));
  orx  g113(.A(n147), .B(n146), .O(n145));
  andx g114(.A(n153), .B(n157), .O(n146));
  andx g115(.A(n152), .B(n148), .O(n147));
  orx  g116(.A(n150), .B(n149), .O(n148));
  andx g117(.A(pi19), .B(pi20), .O(n149));
  andx g118(.A(pi21), .B(n151), .O(n150));
  orx  g119(.A(pi19), .B(pi20), .O(n151));
  orx  g120(.A(n157), .B(n153), .O(n152));
  orx  g121(.A(n155), .B(n154), .O(n153));
  andx g122(.A(pi16), .B(pi17), .O(n154));
  andx g123(.A(pi18), .B(n156), .O(n155));
  orx  g124(.A(pi16), .B(pi17), .O(n156));
  invx g125(.A(n158), .O(n157));
  orx  g126(.A(n169), .B(n159), .O(n158));
  invx g127(.A(n160), .O(n159));
  orx  g128(.A(n163), .B(n161), .O(n160));
  invx g129(.A(n162), .O(n161));
  orx  g130(.A(n164), .B(pi16), .O(n162));
  andx g131(.A(n164), .B(pi16), .O(n163));
  andx g132(.A(n166), .B(n165), .O(n164));
  orx  g133(.A(n168), .B(pi18), .O(n165));
  invx g134(.A(n167), .O(n166));
  andx g135(.A(pi18), .B(n168), .O(n167));
  invx g136(.A(pi17), .O(n168));
  invx g137(.A(n170), .O(n169));
  orx  g138(.A(n173), .B(n171), .O(n170));
  invx g139(.A(n172), .O(n171));
  orx  g140(.A(n174), .B(pi19), .O(n172));
  andx g141(.A(n174), .B(pi19), .O(n173));
  andx g142(.A(n176), .B(n175), .O(n174));
  orx  g143(.A(n178), .B(pi21), .O(n175));
  invx g144(.A(n177), .O(n176));
  andx g145(.A(pi21), .B(n178), .O(n177));
  invx g146(.A(pi20), .O(n178));
  orx  g147(.A(n181), .B(n180), .O(n179));
  andx g148(.A(n187), .B(n191), .O(n180));
  andx g149(.A(n186), .B(n182), .O(n181));
  orx  g150(.A(n184), .B(n183), .O(n182));
  andx g151(.A(pi13), .B(pi14), .O(n183));
  andx g152(.A(pi15), .B(n185), .O(n184));
  orx  g153(.A(pi13), .B(pi14), .O(n185));
  orx  g154(.A(n191), .B(n187), .O(n186));
  orx  g155(.A(n189), .B(n188), .O(n187));
  andx g156(.A(pi10), .B(pi11), .O(n188));
  andx g157(.A(pi12), .B(n190), .O(n189));
  orx  g158(.A(pi10), .B(pi11), .O(n190));
  invx g159(.A(n192), .O(n191));
  orx  g160(.A(n203), .B(n193), .O(n192));
  invx g161(.A(n194), .O(n193));
  orx  g162(.A(n197), .B(n195), .O(n194));
  invx g163(.A(n196), .O(n195));
  orx  g164(.A(n198), .B(pi10), .O(n196));
  andx g165(.A(n198), .B(pi10), .O(n197));
  andx g166(.A(n200), .B(n199), .O(n198));
  orx  g167(.A(n202), .B(pi12), .O(n199));
  invx g168(.A(n201), .O(n200));
  andx g169(.A(pi12), .B(n202), .O(n201));
  invx g170(.A(pi11), .O(n202));
  invx g171(.A(n204), .O(n203));
  orx  g172(.A(n207), .B(n205), .O(n204));
  invx g173(.A(n206), .O(n205));
  orx  g174(.A(n208), .B(pi13), .O(n206));
  andx g175(.A(n208), .B(pi13), .O(n207));
  andx g176(.A(n210), .B(n209), .O(n208));
  orx  g177(.A(n212), .B(pi15), .O(n209));
  invx g178(.A(n211), .O(n210));
  andx g179(.A(pi15), .B(n212), .O(n211));
  invx g180(.A(pi14), .O(n212));
  orx  g181(.A(n297), .B(n214), .O(n213));
  orx  g182(.A(n216), .B(n215), .O(n214));
  andx g183(.A(n284), .B(n250), .O(n215));
  andx g184(.A(n249), .B(n217), .O(n216));
  orx  g185(.A(n219), .B(n218), .O(n217));
  andx g186(.A(n221), .B(n230), .O(n218));
  andx g187(.A(n239), .B(n220), .O(n219));
  orx  g188(.A(n230), .B(n221), .O(n220));
  orx  g189(.A(n226), .B(n222), .O(n221));
  andx g190(.A(n225), .B(n223), .O(n222));
  andx g191(.A(n224), .B(n257), .O(n223));
  orx  g192(.A(n229), .B(n263), .O(n224));
  invx g193(.A(n253), .O(n225));
  andx g194(.A(n227), .B(n253), .O(n226));
  orx  g195(.A(n228), .B(n251), .O(n227));
  andx g196(.A(n229), .B(n263), .O(n228));
  invx g197(.A(n258), .O(n229));
  andx g198(.A(n234), .B(n231), .O(n230));
  orx  g199(.A(n233), .B(n232), .O(n231));
  andx g200(.A(n265), .B(n274), .O(n232));
  andx g201(.A(n275), .B(n264), .O(n233));
  invx g202(.A(n235), .O(n234));
  andx g203(.A(n237), .B(n236), .O(n235));
  orx  g204(.A(n310), .B(n319), .O(n236));
  orx  g205(.A(n238), .B(n309), .O(n237));
  invx g206(.A(n319), .O(n238));
  orx  g207(.A(n244), .B(n240), .O(n239));
  andx g208(.A(n243), .B(n241), .O(n240));
  andx g209(.A(n303), .B(n242), .O(n241));
  orx  g210(.A(n247), .B(n248), .O(n242));
  invx g211(.A(n322), .O(n243));
  andx g212(.A(n322), .B(n245), .O(n244));
  orx  g213(.A(n246), .B(n301), .O(n245));
  andx g214(.A(n248), .B(n247), .O(n246));
  invx g215(.A(n308), .O(n247));
  invx g216(.A(n304), .O(n248));
  orx  g217(.A(n284), .B(n250), .O(n249));
  orx  g218(.A(n252), .B(n251), .O(n250));
  andx g219(.A(n258), .B(n262), .O(n251));
  andx g220(.A(n257), .B(n253), .O(n252));
  orx  g221(.A(n255), .B(n254), .O(n253));
  andx g222(.A(pi25), .B(pi26), .O(n254));
  andx g223(.A(pi27), .B(n256), .O(n255));
  orx  g224(.A(pi25), .B(pi26), .O(n256));
  orx  g225(.A(n262), .B(n258), .O(n257));
  orx  g226(.A(n260), .B(n259), .O(n258));
  andx g227(.A(pi22), .B(pi23), .O(n259));
  andx g228(.A(pi24), .B(n261), .O(n260));
  orx  g229(.A(pi22), .B(pi23), .O(n261));
  invx g230(.A(n263), .O(n262));
  orx  g231(.A(n274), .B(n264), .O(n263));
  invx g232(.A(n265), .O(n264));
  orx  g233(.A(n268), .B(n266), .O(n265));
  invx g234(.A(n267), .O(n266));
  orx  g235(.A(n269), .B(pi22), .O(n267));
  andx g236(.A(n269), .B(pi22), .O(n268));
  andx g237(.A(n271), .B(n270), .O(n269));
  orx  g238(.A(n273), .B(pi24), .O(n270));
  invx g239(.A(n272), .O(n271));
  andx g240(.A(pi24), .B(n273), .O(n272));
  invx g241(.A(pi23), .O(n273));
  invx g242(.A(n275), .O(n274));
  orx  g243(.A(n278), .B(n276), .O(n275));
  invx g244(.A(n277), .O(n276));
  orx  g245(.A(n279), .B(pi25), .O(n277));
  andx g246(.A(n279), .B(pi25), .O(n278));
  andx g247(.A(n281), .B(n280), .O(n279));
  orx  g248(.A(n283), .B(pi27), .O(n280));
  invx g249(.A(n282), .O(n281));
  andx g250(.A(pi27), .B(n283), .O(n282));
  invx g251(.A(pi26), .O(n283));
  invx g252(.A(n285), .O(n284));
  orx  g253(.A(n291), .B(n286), .O(n285));
  andx g254(.A(n290), .B(n287), .O(n286));
  andx g255(.A(n289), .B(n288), .O(n287));
  orx  g256(.A(n294), .B(n332), .O(n288));
  orx  g257(.A(n296), .B(n366), .O(n289));
  invx g258(.A(n300), .O(n290));
  andx g259(.A(n292), .B(n300), .O(n291));
  orx  g260(.A(n295), .B(n293), .O(n292));
  andx g261(.A(n294), .B(n332), .O(n293));
  invx g262(.A(n366), .O(n294));
  andx g263(.A(n296), .B(n366), .O(n295));
  invx g264(.A(n332), .O(n296));
  orx  g265(.A(n299), .B(n298), .O(n297));
  andx g266(.A(n366), .B(n332), .O(n298));
  andx g267(.A(n331), .B(n300), .O(n299));
  orx  g268(.A(n302), .B(n301), .O(n300));
  andx g269(.A(n304), .B(n308), .O(n301));
  andx g270(.A(n322), .B(n303), .O(n302));
  orx  g271(.A(n308), .B(n304), .O(n303));
  andx g272(.A(n306), .B(n305), .O(n304));
  orx  g273(.A(n368), .B(n382), .O(n305));
  orx  g274(.A(n383), .B(n307), .O(n306));
  invx g275(.A(n368), .O(n307));
  andx g276(.A(n319), .B(n309), .O(n308));
  invx g277(.A(n310), .O(n309));
  andx g278(.A(n312), .B(n311), .O(n310));
  orx  g279(.A(n314), .B(n372), .O(n311));
  orx  g280(.A(n373), .B(n313), .O(n312));
  invx g281(.A(n314), .O(n313));
  andx g282(.A(n317), .B(n315), .O(n314));
  orx  g283(.A(n316), .B(pi29), .O(n315));
  invx g284(.A(pi28), .O(n316));
  orx  g285(.A(n318), .B(pi28), .O(n317));
  invx g286(.A(pi29), .O(n318));
  orx  g287(.A(n321), .B(n320), .O(n319));
  andx g288(.A(n347), .B(n356), .O(n320));
  andx g289(.A(n357), .B(n346), .O(n321));
  orx  g290(.A(n327), .B(n323), .O(n322));
  andx g291(.A(n326), .B(n324), .O(n323));
  andx g292(.A(n325), .B(n339), .O(n324));
  orx  g293(.A(n330), .B(n345), .O(n325));
  invx g294(.A(n335), .O(n326));
  andx g295(.A(n328), .B(n335), .O(n327));
  orx  g296(.A(n329), .B(n333), .O(n328));
  andx g297(.A(n330), .B(n345), .O(n329));
  invx g298(.A(n340), .O(n330));
  orx  g299(.A(n366), .B(n332), .O(n331));
  orx  g300(.A(n334), .B(n333), .O(n332));
  andx g301(.A(n340), .B(n344), .O(n333));
  andx g302(.A(n339), .B(n335), .O(n334));
  orx  g303(.A(n337), .B(n336), .O(n335));
  andx g304(.A(pi07), .B(pi08), .O(n336));
  andx g305(.A(pi09), .B(n338), .O(n337));
  orx  g306(.A(pi07), .B(pi08), .O(n338));
  orx  g307(.A(n344), .B(n340), .O(n339));
  orx  g308(.A(n342), .B(n341), .O(n340));
  andx g309(.A(pi04), .B(pi05), .O(n341));
  andx g310(.A(pi06), .B(n343), .O(n342));
  orx  g311(.A(pi04), .B(pi05), .O(n343));
  invx g312(.A(n345), .O(n344));
  orx  g313(.A(n356), .B(n346), .O(n345));
  invx g314(.A(n347), .O(n346));
  orx  g315(.A(n350), .B(n348), .O(n347));
  invx g316(.A(n349), .O(n348));
  orx  g317(.A(n351), .B(pi04), .O(n349));
  andx g318(.A(n351), .B(pi04), .O(n350));
  andx g319(.A(n353), .B(n352), .O(n351));
  orx  g320(.A(n355), .B(pi06), .O(n352));
  invx g321(.A(n354), .O(n353));
  andx g322(.A(pi06), .B(n355), .O(n354));
  invx g323(.A(pi05), .O(n355));
  invx g324(.A(n357), .O(n356));
  orx  g325(.A(n360), .B(n358), .O(n357));
  invx g326(.A(n359), .O(n358));
  orx  g327(.A(n361), .B(pi07), .O(n359));
  andx g328(.A(n361), .B(pi07), .O(n360));
  andx g329(.A(n363), .B(n362), .O(n361));
  orx  g330(.A(n365), .B(pi09), .O(n362));
  invx g331(.A(n364), .O(n363));
  andx g332(.A(pi09), .B(n365), .O(n364));
  invx g333(.A(pi08), .O(n365));
  orx  g334(.A(n387), .B(n367), .O(n366));
  andx g335(.A(n382), .B(n368), .O(n367));
  orx  g336(.A(n370), .B(n369), .O(n368));
  andx g337(.A(pi28), .B(n372), .O(n369));
  andx g338(.A(pi29), .B(n371), .O(n370));
  orx  g339(.A(pi28), .B(n372), .O(n371));
  invx g340(.A(n373), .O(n372));
  andx g341(.A(n375), .B(n374), .O(n373));
  orx  g342(.A(n377), .B(n393), .O(n374));
  orx  g343(.A(n394), .B(n376), .O(n375));
  invx g344(.A(n377), .O(n376));
  andx g345(.A(n380), .B(n378), .O(n377));
  orx  g346(.A(n379), .B(pi03), .O(n378));
  invx g347(.A(pi02), .O(n379));
  orx  g348(.A(n381), .B(pi02), .O(n380));
  invx g349(.A(pi03), .O(n381));
  invx g350(.A(n383), .O(n382));
  orx  g351(.A(n387), .B(n384), .O(n383));
  andx g352(.A(n386), .B(n385), .O(n384));
  orx  g353(.A(n396), .B(n393), .O(n385));
  invx g354(.A(n389), .O(n386));
  andx g355(.A(n394), .B(n388), .O(n387));
  andx g356(.A(n389), .B(pi01), .O(n388));
  orx  g357(.A(n391), .B(n390), .O(n389));
  andx g358(.A(pi02), .B(n393), .O(n390));
  andx g359(.A(pi03), .B(n392), .O(n391));
  orx  g360(.A(pi02), .B(n393), .O(n392));
  invx g361(.A(n394), .O(n393));
  andx g362(.A(n397), .B(n395), .O(n394));
  orx  g363(.A(n396), .B(n32), .O(n395));
  invx g364(.A(pi01), .O(n396));
  orx  g365(.A(n398), .B(pi01), .O(n397));
  invx g366(.A(n32), .O(n398));
endmodule


