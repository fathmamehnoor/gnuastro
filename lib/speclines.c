/*********************************************************************
Spectral lines.
This is part of GNU Astronomy Utilities (Gnuastro) package.

Original author:
     Mohammad Akhlaghi <mohammad@akhlaghi.org>
Contributing author(s):
Copyright (C) 2019-2023 Free Software Foundation, Inc.

Gnuastro is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Gnuastro is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gnuastro. If not, see <http://www.gnu.org/licenses/>.
**********************************************************************/
#include <config.h>

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>

#include <gnuastro/speclines.h>


/*********************************************************************/
/*************        Internal names and codes         ***************/
/*********************************************************************/
/* Return line's name as literal string. */
char *
gal_speclines_line_name(int linecode)
{
  switch(linecode)
    {
    case GAL_SPECLINES_Ne_VIII_770:    return GAL_SPECLINES_NAME_Ne_VIII_770;
    case GAL_SPECLINES_Ne_VIII_780:    return GAL_SPECLINES_NAME_Ne_VIII_780;
    case GAL_SPECLINES_Ly_epsilon:     return GAL_SPECLINES_NAME_Ly_epsilon;
    case GAL_SPECLINES_Ly_delta:       return GAL_SPECLINES_NAME_Ly_delta;
    case GAL_SPECLINES_Ly_gamma:       return GAL_SPECLINES_NAME_Ly_gamma;
    case GAL_SPECLINES_C_III_977:      return GAL_SPECLINES_NAME_C_III_977;
    case GAL_SPECLINES_N_III_989:      return GAL_SPECLINES_NAME_N_III_989;
    case GAL_SPECLINES_N_III_991_51:   return GAL_SPECLINES_NAME_N_III_991_51;
    case GAL_SPECLINES_N_III_991_57:   return GAL_SPECLINES_NAME_N_III_991_57;
    case GAL_SPECLINES_Ly_beta:        return GAL_SPECLINES_NAME_Ly_beta;
    case GAL_SPECLINES_O_VI_1031:      return GAL_SPECLINES_NAME_O_VI_1031;
    case GAL_SPECLINES_O_VI_1037:      return GAL_SPECLINES_NAME_O_VI_1037;
    case GAL_SPECLINES_Ar_I_1066:      return GAL_SPECLINES_NAME_Ar_I_1066;
    case GAL_SPECLINES_Ly_alpha:       return GAL_SPECLINES_NAME_Ly_alpha;
    case GAL_SPECLINES_N_V_1238:       return GAL_SPECLINES_NAME_N_V_1238;
    case GAL_SPECLINES_N_V_1242:       return GAL_SPECLINES_NAME_N_V_1242;
    case GAL_SPECLINES_Si_II_1260:     return GAL_SPECLINES_NAME_Si_II_1260;
    case GAL_SPECLINES_Si_II_1264:     return GAL_SPECLINES_NAME_Si_II_1264;
    case GAL_SPECLINES_O_I_1302:       return GAL_SPECLINES_NAME_O_I_1302;
    case GAL_SPECLINES_C_II_1334:      return GAL_SPECLINES_NAME_C_II_1334;
    case GAL_SPECLINES_C_II_1335:      return GAL_SPECLINES_NAME_C_II_1335;
    case GAL_SPECLINES_Si_IV_1393:     return GAL_SPECLINES_NAME_Si_IV_1393;
    case GAL_SPECLINES_O_IV_1397:      return GAL_SPECLINES_NAME_O_IV_1397;
    case GAL_SPECLINES_O_IV_1399:      return GAL_SPECLINES_NAME_O_IV_1399;
    case GAL_SPECLINES_Si_IV_1402:     return GAL_SPECLINES_NAME_Si_IV_1402;
    case GAL_SPECLINES_N_IV_1486:      return GAL_SPECLINES_NAME_N_IV_1486;
    case GAL_SPECLINES_C_IV_1548:      return GAL_SPECLINES_NAME_C_IV_1548;
    case GAL_SPECLINES_C_IV_1550:      return GAL_SPECLINES_NAME_C_IV_1550;
    case GAL_SPECLINES_He_II_1640:     return GAL_SPECLINES_NAME_He_II_1640;
    case GAL_SPECLINES_O_III_1660:     return GAL_SPECLINES_NAME_O_III_1660;
    case GAL_SPECLINES_O_III_1666:     return GAL_SPECLINES_NAME_O_III_1666;
    case GAL_SPECLINES_N_III_1746:     return GAL_SPECLINES_NAME_N_III_1746;
    case GAL_SPECLINES_N_III_1748:     return GAL_SPECLINES_NAME_N_III_1748;
    case GAL_SPECLINES_Al_III_1854:    return GAL_SPECLINES_NAME_Al_III_1854;
    case GAL_SPECLINES_Al_III_1862:    return GAL_SPECLINES_NAME_Al_III_1862;
    case GAL_SPECLINES_Si_III:         return GAL_SPECLINES_NAME_Si_III;
    case GAL_SPECLINES_C_III_1908:     return GAL_SPECLINES_NAME_C_III_1908;
    case GAL_SPECLINES_N_II_2142:      return GAL_SPECLINES_NAME_N_II_2142;
    case GAL_SPECLINES_O_III_2320:     return GAL_SPECLINES_NAME_O_III_2320;
    case GAL_SPECLINES_C_II_2323:      return GAL_SPECLINES_NAME_C_II_2323;
    case GAL_SPECLINES_C_II_2324:      return GAL_SPECLINES_NAME_C_II_2324;
    case GAL_SPECLINES_Fe_XI_2648:     return GAL_SPECLINES_NAME_Fe_XI_2648;
    case GAL_SPECLINES_He_II_2733:     return GAL_SPECLINES_NAME_He_II_2733;
    case GAL_SPECLINES_Mg_V_2782:      return GAL_SPECLINES_NAME_Mg_V_2782;
    case GAL_SPECLINES_Mg_II_2795:     return GAL_SPECLINES_NAME_Mg_II_2795;
    case GAL_SPECLINES_Mg_II_2802:     return GAL_SPECLINES_NAME_Mg_II_2802;
    case GAL_SPECLINES_Fe_IV_2829:     return GAL_SPECLINES_NAME_Fe_IV_2829;
    case GAL_SPECLINES_Fe_IV_2835:     return GAL_SPECLINES_NAME_Fe_IV_2835;
    case GAL_SPECLINES_Ar_IV_2853:     return GAL_SPECLINES_NAME_Ar_IV_2853;
    case GAL_SPECLINES_Ar_IV_2868:     return GAL_SPECLINES_NAME_Ar_IV_2868;
    case GAL_SPECLINES_Mg_V_2928:      return GAL_SPECLINES_NAME_Mg_V_2928;
    case GAL_SPECLINES_He_I_2945:      return GAL_SPECLINES_NAME_He_I_2945;
    case GAL_SPECLINES_O_III_3132:     return GAL_SPECLINES_NAME_O_III_3132;
    case GAL_SPECLINES_He_I_3187:      return GAL_SPECLINES_NAME_He_I_3187;
    case GAL_SPECLINES_He_II_3203:     return GAL_SPECLINES_NAME_He_II_3203;
    case GAL_SPECLINES_O_III_3312:     return GAL_SPECLINES_NAME_O_III_3312;
    case GAL_SPECLINES_Ne_V_3345:      return GAL_SPECLINES_NAME_Ne_V_3345;
    case GAL_SPECLINES_Ne_V_3425:      return GAL_SPECLINES_NAME_Ne_V_3425;
    case GAL_SPECLINES_O_III_3444:     return GAL_SPECLINES_NAME_O_III_3444;
    case GAL_SPECLINES_N_I_3466_4:     return GAL_SPECLINES_NAME_N_I_3466_4;
    case GAL_SPECLINES_N_I_3466_5:     return GAL_SPECLINES_NAME_N_I_3466_5;
    case GAL_SPECLINES_He_I_3487:      return GAL_SPECLINES_NAME_He_I_3487;
    case GAL_SPECLINES_Fe_VII_3586:    return GAL_SPECLINES_NAME_Fe_VII_3586;
    case GAL_SPECLINES_Fe_VI_3662:     return GAL_SPECLINES_NAME_Fe_VI_3662;
    case GAL_SPECLINES_H_19:           return GAL_SPECLINES_NAME_H_19;
    case GAL_SPECLINES_H_18:           return GAL_SPECLINES_NAME_H_18;
    case GAL_SPECLINES_H_17:           return GAL_SPECLINES_NAME_H_17;
    case GAL_SPECLINES_H_16:           return GAL_SPECLINES_NAME_H_16;
    case GAL_SPECLINES_H_15:           return GAL_SPECLINES_NAME_H_15;
    case GAL_SPECLINES_H_14:           return GAL_SPECLINES_NAME_H_14;
    case GAL_SPECLINES_O_II_3726:      return GAL_SPECLINES_NAME_O_II_3726;
    case GAL_SPECLINES_O_II_3728:      return GAL_SPECLINES_NAME_O_II_3728;
    case GAL_SPECLINES_H_13:           return GAL_SPECLINES_NAME_H_13;
    case GAL_SPECLINES_H_12:           return GAL_SPECLINES_NAME_H_12;
    case GAL_SPECLINES_Fe_VII_3758:    return GAL_SPECLINES_NAME_Fe_VII_3758;
    case GAL_SPECLINES_H_11:           return GAL_SPECLINES_NAME_H_11;
    case GAL_SPECLINES_H_10:           return GAL_SPECLINES_NAME_H_10;
    case GAL_SPECLINES_H_9:            return GAL_SPECLINES_NAME_H_9;
    case GAL_SPECLINES_Fe_V_3839:      return GAL_SPECLINES_NAME_Fe_V_3839;
    case GAL_SPECLINES_Ne_III_3868:    return GAL_SPECLINES_NAME_Ne_III_3868;
    case GAL_SPECLINES_He_I_3888:      return GAL_SPECLINES_NAME_He_I_3888;
    case GAL_SPECLINES_H_8:            return GAL_SPECLINES_NAME_H_8;
    case GAL_SPECLINES_Fe_V_3891:      return GAL_SPECLINES_NAME_Fe_V_3891;
    case GAL_SPECLINES_Fe_V_3911:      return GAL_SPECLINES_NAME_Fe_V_3911;
    case GAL_SPECLINES_Ne_III_3967:    return GAL_SPECLINES_NAME_Ne_III_3967;
    case GAL_SPECLINES_H_epsilon:      return GAL_SPECLINES_NAME_H_epsilon;
    case GAL_SPECLINES_He_I_4026:      return GAL_SPECLINES_NAME_He_I_4026;
    case GAL_SPECLINES_S_II_4068:      return GAL_SPECLINES_NAME_S_II_4068;
    case GAL_SPECLINES_Fe_V_4071:      return GAL_SPECLINES_NAME_Fe_V_4071;
    case GAL_SPECLINES_S_II_4076:      return GAL_SPECLINES_NAME_S_II_4076;
    case GAL_SPECLINES_H_delta:        return GAL_SPECLINES_NAME_H_delta;
    case GAL_SPECLINES_He_I_4143:      return GAL_SPECLINES_NAME_He_I_4143;
    case GAL_SPECLINES_Fe_II_4178:     return GAL_SPECLINES_NAME_Fe_II_4178;
    case GAL_SPECLINES_Fe_V_4180:      return GAL_SPECLINES_NAME_Fe_V_4180;
    case GAL_SPECLINES_Fe_II_4233:     return GAL_SPECLINES_NAME_Fe_II_4233;
    case GAL_SPECLINES_Fe_V_4227:      return GAL_SPECLINES_NAME_Fe_V_4227;
    case GAL_SPECLINES_Fe_II_4287:     return GAL_SPECLINES_NAME_Fe_II_4287;
    case GAL_SPECLINES_Fe_II_4304:     return GAL_SPECLINES_NAME_Fe_II_4304;
    case GAL_SPECLINES_O_II_4317:      return GAL_SPECLINES_NAME_O_II_4317;
    case GAL_SPECLINES_H_gamma:        return GAL_SPECLINES_NAME_H_gamma;
    case GAL_SPECLINES_O_III_4363:     return GAL_SPECLINES_NAME_O_III_4363;
    case GAL_SPECLINES_Ar_XIV:         return GAL_SPECLINES_NAME_Ar_XIV;
    case GAL_SPECLINES_O_II_4414:      return GAL_SPECLINES_NAME_O_II_4414;
    case GAL_SPECLINES_Fe_II_4416:     return GAL_SPECLINES_NAME_Fe_II_4416;
    case GAL_SPECLINES_Fe_II_4452:     return GAL_SPECLINES_NAME_Fe_II_4452;
    case GAL_SPECLINES_He_I_4471:      return GAL_SPECLINES_NAME_He_I_4471;
    case GAL_SPECLINES_Fe_II_4489:     return GAL_SPECLINES_NAME_Fe_II_4489;
    case GAL_SPECLINES_Fe_II_4491:     return GAL_SPECLINES_NAME_Fe_II_4491;
    case GAL_SPECLINES_N_III_4510:     return GAL_SPECLINES_NAME_N_III_4510;
    case GAL_SPECLINES_Fe_II_4522:     return GAL_SPECLINES_NAME_Fe_II_4522;
    case GAL_SPECLINES_Fe_II_4555:     return GAL_SPECLINES_NAME_Fe_II_4555;
    case GAL_SPECLINES_Fe_II_4582:     return GAL_SPECLINES_NAME_Fe_II_4582;
    case GAL_SPECLINES_Fe_II_4583:     return GAL_SPECLINES_NAME_Fe_II_4583;
    case GAL_SPECLINES_Fe_II_4629:     return GAL_SPECLINES_NAME_Fe_II_4629;
    case GAL_SPECLINES_N_III_4634:     return GAL_SPECLINES_NAME_N_III_4634;
    case GAL_SPECLINES_N_III_4640:     return GAL_SPECLINES_NAME_N_III_4640;
    case GAL_SPECLINES_N_III_4641:     return GAL_SPECLINES_NAME_N_III_4641;
    case GAL_SPECLINES_C_III_4647:     return GAL_SPECLINES_NAME_C_III_4647;
    case GAL_SPECLINES_C_III_4650:     return GAL_SPECLINES_NAME_C_III_4650;
    case GAL_SPECLINES_C_III_5651:     return GAL_SPECLINES_NAME_C_III_5651;
    case GAL_SPECLINES_Fe_III_4658:    return GAL_SPECLINES_NAME_Fe_III_4658;
    case GAL_SPECLINES_He_II_4685:     return GAL_SPECLINES_NAME_He_II_4685;
    case GAL_SPECLINES_Ar_IV_4711:     return GAL_SPECLINES_NAME_Ar_IV_4711;
    case GAL_SPECLINES_Ar_IV_4740:     return GAL_SPECLINES_NAME_Ar_IV_4740;
    case GAL_SPECLINES_H_beta:         return GAL_SPECLINES_NAME_H_beta;
    case GAL_SPECLINES_Fe_VII_4893:    return GAL_SPECLINES_NAME_Fe_VII_4893;
    case GAL_SPECLINES_Fe_IV_4903:     return GAL_SPECLINES_NAME_Fe_IV_4903;
    case GAL_SPECLINES_Fe_II_4923:     return GAL_SPECLINES_NAME_Fe_II_4923;
    case GAL_SPECLINES_O_III_4958:     return GAL_SPECLINES_NAME_O_III_4958;
    case GAL_SPECLINES_O_III_5006:     return GAL_SPECLINES_NAME_O_III_5006;
    case GAL_SPECLINES_Fe_II_5018:     return GAL_SPECLINES_NAME_Fe_II_5018;
    case GAL_SPECLINES_Fe_III_5084:    return GAL_SPECLINES_NAME_Fe_III_5084;
    case GAL_SPECLINES_Fe_VI_5145:     return GAL_SPECLINES_NAME_Fe_VI_5145;
    case GAL_SPECLINES_Fe_VII_5158:    return GAL_SPECLINES_NAME_Fe_VII_5158;
    case GAL_SPECLINES_Fe_II_5169:     return GAL_SPECLINES_NAME_Fe_II_5169;
    case GAL_SPECLINES_Fe_VI_5176:     return GAL_SPECLINES_NAME_Fe_VI_5176;
    case GAL_SPECLINES_Fe_II_5197:     return GAL_SPECLINES_NAME_Fe_II_5197;
    case GAL_SPECLINES_N_I_5200:       return GAL_SPECLINES_NAME_N_I_5200;
    case GAL_SPECLINES_Fe_II_5234:     return GAL_SPECLINES_NAME_Fe_II_5234;
    case GAL_SPECLINES_Fe_IV_5236:     return GAL_SPECLINES_NAME_Fe_IV_5236;
    case GAL_SPECLINES_Fe_III_5270:    return GAL_SPECLINES_NAME_Fe_III_5270;
    case GAL_SPECLINES_Fe_II_5276:     return GAL_SPECLINES_NAME_Fe_II_5276;
    case GAL_SPECLINES_Fe_VII_5276:    return GAL_SPECLINES_NAME_Fe_VII_5276;
    case GAL_SPECLINES_Fe_XIV:         return GAL_SPECLINES_NAME_Fe_XIV;
    case GAL_SPECLINES_Ca_V:           return GAL_SPECLINES_NAME_Ca_V;
    case GAL_SPECLINES_Fe_II_5316_6:   return GAL_SPECLINES_NAME_Fe_II_5316_6;
    case GAL_SPECLINES_Fe_II_5316_7:   return GAL_SPECLINES_NAME_Fe_II_5316_7;
    case GAL_SPECLINES_Fe_VI_5335:     return GAL_SPECLINES_NAME_Fe_VI_5335;
    case GAL_SPECLINES_Fe_VI_5424:     return GAL_SPECLINES_NAME_Fe_VI_5424;
    case GAL_SPECLINES_Cl_III_5517:    return GAL_SPECLINES_NAME_Cl_III_5517;
    case GAL_SPECLINES_Cl_III_5537:    return GAL_SPECLINES_NAME_Cl_III_5537;
    case GAL_SPECLINES_Fe_VI_5637:     return GAL_SPECLINES_NAME_Fe_VI_5637;
    case GAL_SPECLINES_Fe_VI_5677:     return GAL_SPECLINES_NAME_Fe_VI_5677;
    case GAL_SPECLINES_C_III_5697:     return GAL_SPECLINES_NAME_C_III_5697;
    case GAL_SPECLINES_Fe_VII_5720:    return GAL_SPECLINES_NAME_Fe_VII_5720;
    case GAL_SPECLINES_N_II_5754:      return GAL_SPECLINES_NAME_N_II_5754;
    case GAL_SPECLINES_C_IV_5801:      return GAL_SPECLINES_NAME_C_IV_5801;
    case GAL_SPECLINES_C_IV_5811:      return GAL_SPECLINES_NAME_C_IV_5811;
    case GAL_SPECLINES_He_I_5875:      return GAL_SPECLINES_NAME_He_I_5875;
    case GAL_SPECLINES_O_I_6046:       return GAL_SPECLINES_NAME_O_I_6046;
    case GAL_SPECLINES_Fe_VII_6087:    return GAL_SPECLINES_NAME_Fe_VII_6087;
    case GAL_SPECLINES_O_I_6300:       return GAL_SPECLINES_NAME_O_I_6300;
    case GAL_SPECLINES_S_III_6312:     return GAL_SPECLINES_NAME_S_III_6312;
    case GAL_SPECLINES_Si_II_6347:     return GAL_SPECLINES_NAME_Si_II_6347;
    case GAL_SPECLINES_O_I_6363:       return GAL_SPECLINES_NAME_O_I_6363;
    case GAL_SPECLINES_Fe_II_6369:     return GAL_SPECLINES_NAME_Fe_II_6369;
    case GAL_SPECLINES_Fe_X:           return GAL_SPECLINES_NAME_Fe_X;
    case GAL_SPECLINES_Fe_II_6516:     return GAL_SPECLINES_NAME_Fe_II_6516;
    case GAL_SPECLINES_N_II_6548:      return GAL_SPECLINES_NAME_N_II_6548;
    case GAL_SPECLINES_H_alpha:        return GAL_SPECLINES_NAME_H_alpha;
    case GAL_SPECLINES_N_II_6583:      return GAL_SPECLINES_NAME_N_II_6583;
    case GAL_SPECLINES_S_II_6716:      return GAL_SPECLINES_NAME_S_II_6716;
    case GAL_SPECLINES_S_II_6730:      return GAL_SPECLINES_NAME_S_II_6730;
    case GAL_SPECLINES_O_I_7002:       return GAL_SPECLINES_NAME_O_I_7002;
    case GAL_SPECLINES_Ar_V:           return GAL_SPECLINES_NAME_Ar_V;
    case GAL_SPECLINES_He_I_7065:      return GAL_SPECLINES_NAME_He_I_7065;
    case GAL_SPECLINES_Ar_III_7135:    return GAL_SPECLINES_NAME_Ar_III_7135;
    case GAL_SPECLINES_Fe_II_7155:     return GAL_SPECLINES_NAME_Fe_II_7155;
    case GAL_SPECLINES_Ar_IV_7170:     return GAL_SPECLINES_NAME_Ar_IV_7170;
    case GAL_SPECLINES_Fe_II_7172:     return GAL_SPECLINES_NAME_Fe_II_7172;
    case GAL_SPECLINES_C_II_7236:      return GAL_SPECLINES_NAME_C_II_7236;
    case GAL_SPECLINES_Ar_IV_7237:     return GAL_SPECLINES_NAME_Ar_IV_7237;
    case GAL_SPECLINES_O_I_7254:       return GAL_SPECLINES_NAME_O_I_7254;
    case GAL_SPECLINES_Ar_IV_7262:     return GAL_SPECLINES_NAME_Ar_IV_7262;
    case GAL_SPECLINES_He_I_7281:      return GAL_SPECLINES_NAME_He_I_7281;
    case GAL_SPECLINES_O_II_7319:      return GAL_SPECLINES_NAME_O_II_7319;
    case GAL_SPECLINES_O_II_7330:      return GAL_SPECLINES_NAME_O_II_7330;
    case GAL_SPECLINES_Ni_II_7377:     return GAL_SPECLINES_NAME_Ni_II_7377;
    case GAL_SPECLINES_Ni_II_7411:     return GAL_SPECLINES_NAME_Ni_II_7411;
    case GAL_SPECLINES_Fe_II_7452:     return GAL_SPECLINES_NAME_Fe_II_7452;
    case GAL_SPECLINES_N_I_7468:       return GAL_SPECLINES_NAME_N_I_7468;
    case GAL_SPECLINES_S_XII:          return GAL_SPECLINES_NAME_S_XII;
    case GAL_SPECLINES_Ar_III_7751:    return GAL_SPECLINES_NAME_Ar_III_7751;
    case GAL_SPECLINES_He_I_7816:      return GAL_SPECLINES_NAME_He_I_7816;
    case GAL_SPECLINES_Ar_I_7868:      return GAL_SPECLINES_NAME_Ar_I_7868;
    case GAL_SPECLINES_Ni_III:         return GAL_SPECLINES_NAME_Ni_III;
    case GAL_SPECLINES_Fe_XI_7891:     return GAL_SPECLINES_NAME_Fe_XI_7891;
    case GAL_SPECLINES_He_II_8236:     return GAL_SPECLINES_NAME_He_II_8236;
    case GAL_SPECLINES_Pa_20:          return GAL_SPECLINES_NAME_Pa_20;
    case GAL_SPECLINES_Pa_19:          return GAL_SPECLINES_NAME_Pa_19;
    case GAL_SPECLINES_Pa_18:          return GAL_SPECLINES_NAME_Pa_18;
    case GAL_SPECLINES_O_I_8446:       return GAL_SPECLINES_NAME_O_I_8446;
    case GAL_SPECLINES_Pa_17:          return GAL_SPECLINES_NAME_Pa_17;
    case GAL_SPECLINES_Ca_II_8498:     return GAL_SPECLINES_NAME_Ca_II_8498;
    case GAL_SPECLINES_Pa_16:          return GAL_SPECLINES_NAME_Pa_16;
    case GAL_SPECLINES_Ca_II_8542:     return GAL_SPECLINES_NAME_Ca_II_8542;
    case GAL_SPECLINES_Pa_15:          return GAL_SPECLINES_NAME_Pa_15;
    case GAL_SPECLINES_Cl_II:          return GAL_SPECLINES_NAME_Cl_II;
    case GAL_SPECLINES_Pa_14:          return GAL_SPECLINES_NAME_Pa_14;
    case GAL_SPECLINES_Fe_II_8616:     return GAL_SPECLINES_NAME_Fe_II_8616;
    case GAL_SPECLINES_Ca_II_8662:     return GAL_SPECLINES_NAME_Ca_II_8662;
    case GAL_SPECLINES_Pa_13:          return GAL_SPECLINES_NAME_Pa_13;
    case GAL_SPECLINES_N_I_8680:       return GAL_SPECLINES_NAME_N_I_8680;
    case GAL_SPECLINES_N_I_8703:       return GAL_SPECLINES_NAME_N_I_8703;
    case GAL_SPECLINES_N_I_8711:       return GAL_SPECLINES_NAME_N_I_8711;
    case GAL_SPECLINES_Pa_12:          return GAL_SPECLINES_NAME_Pa_12;
    case GAL_SPECLINES_Pa_11:          return GAL_SPECLINES_NAME_Pa_11;
    case GAL_SPECLINES_Fe_II_8891:     return GAL_SPECLINES_NAME_Fe_II_8891;
    case GAL_SPECLINES_Pa_10:          return GAL_SPECLINES_NAME_Pa_10;
    case GAL_SPECLINES_S_III_9068:     return GAL_SPECLINES_NAME_S_III_9068;
    case GAL_SPECLINES_Pa_9:           return GAL_SPECLINES_NAME_Pa_9;
    case GAL_SPECLINES_S_III_9531:     return GAL_SPECLINES_NAME_S_III_9531;
    case GAL_SPECLINES_Pa_epsilon:     return GAL_SPECLINES_NAME_Pa_epsilon;
    case GAL_SPECLINES_C_I_9824:       return GAL_SPECLINES_NAME_C_I_9824;
    case GAL_SPECLINES_C_I_9850:       return GAL_SPECLINES_NAME_C_I_9850;
    case GAL_SPECLINES_S_VIII:         return GAL_SPECLINES_NAME_S_VIII;
    case GAL_SPECLINES_He_I_10027:     return GAL_SPECLINES_NAME_He_I_10027;
    case GAL_SPECLINES_He_I_10031:     return GAL_SPECLINES_NAME_He_I_10031;
    case GAL_SPECLINES_Pa_delta:       return GAL_SPECLINES_NAME_Pa_delta;
    case GAL_SPECLINES_S_II_10286:     return GAL_SPECLINES_NAME_S_II_10286;
    case GAL_SPECLINES_S_II_10320:     return GAL_SPECLINES_NAME_S_II_10320;
    case GAL_SPECLINES_S_II_10336:     return GAL_SPECLINES_NAME_S_II_10336;
    case GAL_SPECLINES_Fe_XIII:        return GAL_SPECLINES_NAME_Fe_XIII;
    case GAL_SPECLINES_He_I_10830:     return GAL_SPECLINES_NAME_He_I_10830;
    case GAL_SPECLINES_Pa_gamma:       return GAL_SPECLINES_NAME_Pa_gamma;

    /* Limits */
    case GAL_SPECLINES_LIMIT_LYMAN:    return GAL_SPECLINES_NAME_LIMIT_LYMAN;
    case GAL_SPECLINES_LIMIT_BALMER:   return GAL_SPECLINES_NAME_LIMIT_BALMER;
    case GAL_SPECLINES_LIMIT_PASCHEN:  return GAL_SPECLINES_NAME_LIMIT_PASCHEN;
    default: return NULL;
    }
  return NULL;
}





/* Return the code of the given line name. */
int
gal_speclines_line_code(char *name)
{
  if( !strcmp(name, GAL_SPECLINES_NAME_Ne_VIII_770) )
    return GAL_SPECLINES_Ne_VIII_770;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ne_VIII_780) )
    return GAL_SPECLINES_Ne_VIII_780;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ly_epsilon) )
    return GAL_SPECLINES_Ly_epsilon;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ly_delta) )
    return GAL_SPECLINES_Ly_delta;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ly_gamma) )
    return GAL_SPECLINES_Ly_gamma;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_III_977) )
    return GAL_SPECLINES_C_III_977;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_989) )
    return GAL_SPECLINES_N_III_989;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_991_51) )
    return GAL_SPECLINES_N_III_991_51;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_991_57) )
    return GAL_SPECLINES_N_III_991_57;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ly_beta) )
    return GAL_SPECLINES_Ly_beta;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_VI_1031) )
    return GAL_SPECLINES_O_VI_1031;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_VI_1037) )
    return GAL_SPECLINES_O_VI_1037;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_I_1066) )
    return GAL_SPECLINES_Ar_I_1066;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ly_alpha) )
    return GAL_SPECLINES_Ly_alpha;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_V_1238) )
    return GAL_SPECLINES_N_V_1238;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_V_1242) )
    return GAL_SPECLINES_N_V_1242;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Si_II_1260) )
    return GAL_SPECLINES_Si_II_1260;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Si_II_1264) )
    return GAL_SPECLINES_Si_II_1264;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_1302) )
    return GAL_SPECLINES_O_I_1302;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_II_1334) )
    return GAL_SPECLINES_C_II_1334;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_II_1335) )
    return GAL_SPECLINES_C_II_1335;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Si_IV_1393) )
    return GAL_SPECLINES_Si_IV_1393;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_IV_1397) )
    return GAL_SPECLINES_O_IV_1397;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_IV_1399) )
    return GAL_SPECLINES_O_IV_1399;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Si_IV_1402) )
    return GAL_SPECLINES_Si_IV_1402;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_IV_1486) )
    return GAL_SPECLINES_N_IV_1486;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_IV_1548) )
    return GAL_SPECLINES_C_IV_1548;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_IV_1550) )
    return GAL_SPECLINES_C_IV_1550;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_II_1640) )
    return GAL_SPECLINES_He_II_1640;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_1660) )
    return GAL_SPECLINES_O_III_1660;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_1666) )
    return GAL_SPECLINES_O_III_1666;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_1746) )
    return GAL_SPECLINES_N_III_1746;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_1748) )
    return GAL_SPECLINES_N_III_1748;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Al_III_1854) )
    return GAL_SPECLINES_Al_III_1854;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Al_III_1862) )
    return GAL_SPECLINES_Al_III_1862;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Si_III) )
    return GAL_SPECLINES_Si_III;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_III_1908) )
    return GAL_SPECLINES_C_III_1908;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_II_2142) )
    return GAL_SPECLINES_N_II_2142;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_2320) )
    return GAL_SPECLINES_O_III_2320;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_II_2323) )
    return GAL_SPECLINES_C_II_2323;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_II_2324) )
    return GAL_SPECLINES_C_II_2324;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_XI_2648) )
    return GAL_SPECLINES_Fe_XI_2648;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_II_2733) )
    return GAL_SPECLINES_He_II_2733;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Mg_V_2782) )
    return GAL_SPECLINES_Mg_V_2782;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Mg_II_2795) )
    return GAL_SPECLINES_Mg_II_2795;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Mg_II_2802) )
    return GAL_SPECLINES_Mg_II_2802;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_IV_2829) )
    return GAL_SPECLINES_Fe_IV_2829;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_IV_2835) )
    return GAL_SPECLINES_Fe_IV_2835;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_2853) )
    return GAL_SPECLINES_Ar_IV_2853;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_2868) )
    return GAL_SPECLINES_Ar_IV_2868;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Mg_V_2928) )
    return GAL_SPECLINES_Mg_V_2928;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_2945) )
    return GAL_SPECLINES_He_I_2945;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_3132) )
    return GAL_SPECLINES_O_III_3132;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_3187) )
    return GAL_SPECLINES_He_I_3187;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_II_3203) )
    return GAL_SPECLINES_He_II_3203;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_3312) )
    return GAL_SPECLINES_O_III_3312;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ne_V_3345) )
    return GAL_SPECLINES_Ne_V_3345;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ne_V_3425) )
    return GAL_SPECLINES_Ne_V_3425;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_3444) )
    return GAL_SPECLINES_O_III_3444;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_3466_4) )
    return GAL_SPECLINES_N_I_3466_4;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_3466_5) )
    return GAL_SPECLINES_N_I_3466_5;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_3487) )
    return GAL_SPECLINES_He_I_3487;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_3586) )
    return GAL_SPECLINES_Fe_VII_3586;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_3662) )
    return GAL_SPECLINES_Fe_VI_3662;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_19) )
    return GAL_SPECLINES_H_19;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_18) )
    return GAL_SPECLINES_H_18;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_17) )
    return GAL_SPECLINES_H_17;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_16) )
    return GAL_SPECLINES_H_16;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_15) )
    return GAL_SPECLINES_H_15;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_14) )
    return GAL_SPECLINES_H_14;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_II_3726) )
    return GAL_SPECLINES_O_II_3726;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_II_3728) )
    return GAL_SPECLINES_O_II_3728;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_13) )
    return GAL_SPECLINES_H_13;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_12) )
    return GAL_SPECLINES_H_12;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_3758) )
    return GAL_SPECLINES_Fe_VII_3758;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_11) )
    return GAL_SPECLINES_H_11;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_10) )
    return GAL_SPECLINES_H_10;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_9) )
    return GAL_SPECLINES_H_9;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_V_3839) )
    return GAL_SPECLINES_Fe_V_3839;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ne_III_3868) )
    return GAL_SPECLINES_Ne_III_3868;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_3888) )
    return GAL_SPECLINES_He_I_3888;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_8) )
    return GAL_SPECLINES_H_8;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_V_3891) )
    return GAL_SPECLINES_Fe_V_3891;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_V_3911) )
    return GAL_SPECLINES_Fe_V_3911;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ne_III_3967) )
    return GAL_SPECLINES_Ne_III_3967;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_epsilon) )
    return GAL_SPECLINES_H_epsilon;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_4026) )
    return GAL_SPECLINES_He_I_4026;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_4068) )
    return GAL_SPECLINES_S_II_4068;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_V_4071) )
    return GAL_SPECLINES_Fe_V_4071;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_4076) )
    return GAL_SPECLINES_S_II_4076;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_delta) )
    return GAL_SPECLINES_H_delta;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_4143) )
    return GAL_SPECLINES_He_I_4143;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4178) )
    return GAL_SPECLINES_Fe_II_4178;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_V_4180) )
    return GAL_SPECLINES_Fe_V_4180;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4233) )
    return GAL_SPECLINES_Fe_II_4233;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_V_4227) )
    return GAL_SPECLINES_Fe_V_4227;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4287) )
    return GAL_SPECLINES_Fe_II_4287;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4304) )
    return GAL_SPECLINES_Fe_II_4304;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_II_4317) )
    return GAL_SPECLINES_O_II_4317;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_gamma) )
    return GAL_SPECLINES_H_gamma;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_4363) )
    return GAL_SPECLINES_O_III_4363;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_XIV) )
    return GAL_SPECLINES_Ar_XIV;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_II_4414) )
    return GAL_SPECLINES_O_II_4414;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4416) )
    return GAL_SPECLINES_Fe_II_4416;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4452) )
    return GAL_SPECLINES_Fe_II_4452;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_4471) )
    return GAL_SPECLINES_He_I_4471;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4489) )
    return GAL_SPECLINES_Fe_II_4489;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4491) )
    return GAL_SPECLINES_Fe_II_4491;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_4510) )
    return GAL_SPECLINES_N_III_4510;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4522) )
    return GAL_SPECLINES_Fe_II_4522;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4555) )
    return GAL_SPECLINES_Fe_II_4555;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4582) )
    return GAL_SPECLINES_Fe_II_4582;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4583) )
    return GAL_SPECLINES_Fe_II_4583;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4629) )
    return GAL_SPECLINES_Fe_II_4629;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_4634) )
    return GAL_SPECLINES_N_III_4634;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_4640) )
    return GAL_SPECLINES_N_III_4640;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_III_4641) )
    return GAL_SPECLINES_N_III_4641;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_III_4647) )
    return GAL_SPECLINES_C_III_4647;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_III_4650) )
    return GAL_SPECLINES_C_III_4650;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_III_5651) )
    return GAL_SPECLINES_C_III_5651;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_III_4658) )
    return GAL_SPECLINES_Fe_III_4658;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_II_4685) )
    return GAL_SPECLINES_He_II_4685;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_4711) )
    return GAL_SPECLINES_Ar_IV_4711;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_4740) )
    return GAL_SPECLINES_Ar_IV_4740;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_beta) )
    return GAL_SPECLINES_H_beta;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_4893) )
    return GAL_SPECLINES_Fe_VII_4893;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_IV_4903) )
    return GAL_SPECLINES_Fe_IV_4903;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_4923) )
    return GAL_SPECLINES_Fe_II_4923;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_4958) )
    return GAL_SPECLINES_O_III_4958;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_III_5006) )
    return GAL_SPECLINES_O_III_5006;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5018) )
    return GAL_SPECLINES_Fe_II_5018;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_III_5084) )
    return GAL_SPECLINES_Fe_III_5084;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_5145) )
    return GAL_SPECLINES_Fe_VI_5145;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_5158) )
    return GAL_SPECLINES_Fe_VII_5158;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5169) )
    return GAL_SPECLINES_Fe_II_5169;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_5176) )
    return GAL_SPECLINES_Fe_VI_5176;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5197) )
    return GAL_SPECLINES_Fe_II_5197;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_5200) )
    return GAL_SPECLINES_N_I_5200;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5234) )
    return GAL_SPECLINES_Fe_II_5234;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_IV_5236) )
    return GAL_SPECLINES_Fe_IV_5236;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_III_5270) )
    return GAL_SPECLINES_Fe_III_5270;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5276) )
    return GAL_SPECLINES_Fe_II_5276;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_5276) )
    return GAL_SPECLINES_Fe_VII_5276;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_XIV) )
    return GAL_SPECLINES_Fe_XIV;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ca_V) )
    return GAL_SPECLINES_Ca_V;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5316_6) )
    return GAL_SPECLINES_Fe_II_5316_6;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_5316_7) )
    return GAL_SPECLINES_Fe_II_5316_7;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_5335) )
    return GAL_SPECLINES_Fe_VI_5335;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_5424) )
    return GAL_SPECLINES_Fe_VI_5424;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Cl_III_5517) )
    return GAL_SPECLINES_Cl_III_5517;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Cl_III_5537) )
    return GAL_SPECLINES_Cl_III_5537;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_5637) )
    return GAL_SPECLINES_Fe_VI_5637;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VI_5677) )
    return GAL_SPECLINES_Fe_VI_5677;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_III_5697) )
    return GAL_SPECLINES_C_III_5697;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_5720) )
    return GAL_SPECLINES_Fe_VII_5720;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_II_5754) )
    return GAL_SPECLINES_N_II_5754;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_IV_5801) )
    return GAL_SPECLINES_C_IV_5801;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_IV_5811) )
    return GAL_SPECLINES_C_IV_5811;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_5875) )
    return GAL_SPECLINES_He_I_5875;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_6046) )
    return GAL_SPECLINES_O_I_6046;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_VII_6087) )
    return GAL_SPECLINES_Fe_VII_6087;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_6300) )
    return GAL_SPECLINES_O_I_6300;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_III_6312) )
    return GAL_SPECLINES_S_III_6312;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Si_II_6347) )
    return GAL_SPECLINES_Si_II_6347;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_6363) )
    return GAL_SPECLINES_O_I_6363;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_6369) )
    return GAL_SPECLINES_Fe_II_6369;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_X) )
    return GAL_SPECLINES_Fe_X;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_6516) )
    return GAL_SPECLINES_Fe_II_6516;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_II_6548) )
    return GAL_SPECLINES_N_II_6548;
  else if( !strcmp(name, GAL_SPECLINES_NAME_H_alpha) )
    return GAL_SPECLINES_H_alpha;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_II_6583) )
    return GAL_SPECLINES_N_II_6583;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_6716) )
    return GAL_SPECLINES_S_II_6716;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_6730) )
    return GAL_SPECLINES_S_II_6730;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_7002) )
    return GAL_SPECLINES_O_I_7002;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_V) )
    return GAL_SPECLINES_Ar_V;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_7065) )
    return GAL_SPECLINES_He_I_7065;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_III_7135) )
    return GAL_SPECLINES_Ar_III_7135;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_7155) )
    return GAL_SPECLINES_Fe_II_7155;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_7170) )
    return GAL_SPECLINES_Ar_IV_7170;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_7172) )
    return GAL_SPECLINES_Fe_II_7172;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_II_7236) )
    return GAL_SPECLINES_C_II_7236;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_7237) )
    return GAL_SPECLINES_Ar_IV_7237;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_7254) )
    return GAL_SPECLINES_O_I_7254;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_IV_7262) )
    return GAL_SPECLINES_Ar_IV_7262;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_7281) )
    return GAL_SPECLINES_He_I_7281;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_II_7319) )
    return GAL_SPECLINES_O_II_7319;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_II_7330) )
    return GAL_SPECLINES_O_II_7330;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ni_II_7377) )
    return GAL_SPECLINES_Ni_II_7377;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ni_II_7411) )
    return GAL_SPECLINES_Ni_II_7411;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_7452) )
    return GAL_SPECLINES_Fe_II_7452;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_7468) )
    return GAL_SPECLINES_N_I_7468;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_XII) )
    return GAL_SPECLINES_S_XII;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_III_7751) )
    return GAL_SPECLINES_Ar_III_7751;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_7816) )
    return GAL_SPECLINES_He_I_7816;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ar_I_7868) )
    return GAL_SPECLINES_Ar_I_7868;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ni_III) )
    return GAL_SPECLINES_Ni_III;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_XI_7891) )
    return GAL_SPECLINES_Fe_XI_7891;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_II_8236) )
    return GAL_SPECLINES_He_II_8236;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_20) )
    return GAL_SPECLINES_Pa_20;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_19) )
    return GAL_SPECLINES_Pa_19;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_18) )
    return GAL_SPECLINES_Pa_18;
  else if( !strcmp(name, GAL_SPECLINES_NAME_O_I_8446) )
    return GAL_SPECLINES_O_I_8446;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_17) )
    return GAL_SPECLINES_Pa_17;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ca_II_8498) )
    return GAL_SPECLINES_Ca_II_8498;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_16) )
    return GAL_SPECLINES_Pa_16;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ca_II_8542) )
    return GAL_SPECLINES_Ca_II_8542;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_15) )
    return GAL_SPECLINES_Pa_15;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Cl_II) )
    return GAL_SPECLINES_Cl_II;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_14) )
    return GAL_SPECLINES_Pa_14;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_8616) )
    return GAL_SPECLINES_Fe_II_8616;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Ca_II_8662) )
    return GAL_SPECLINES_Ca_II_8662;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_13) )
    return GAL_SPECLINES_Pa_13;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_8680) )
    return GAL_SPECLINES_N_I_8680;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_8703) )
    return GAL_SPECLINES_N_I_8703;
  else if( !strcmp(name, GAL_SPECLINES_NAME_N_I_8711) )
    return GAL_SPECLINES_N_I_8711;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_12) )
    return GAL_SPECLINES_Pa_12;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_11) )
    return GAL_SPECLINES_Pa_11;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_II_8891) )
    return GAL_SPECLINES_Fe_II_8891;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_10) )
    return GAL_SPECLINES_Pa_10;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_III_9068) )
    return GAL_SPECLINES_S_III_9068;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_9) )
    return GAL_SPECLINES_Pa_9;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_III_9531) )
    return GAL_SPECLINES_S_III_9531;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_epsilon) )
    return GAL_SPECLINES_Pa_epsilon;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_I_9824) )
    return GAL_SPECLINES_C_I_9824;
  else if( !strcmp(name, GAL_SPECLINES_NAME_C_I_9850) )
    return GAL_SPECLINES_C_I_9850;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_VIII) )
    return GAL_SPECLINES_S_VIII;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_10027) )
    return GAL_SPECLINES_He_I_10027;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_10031) )
    return GAL_SPECLINES_He_I_10031;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_delta) )
    return GAL_SPECLINES_Pa_delta;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_10286) )
    return GAL_SPECLINES_S_II_10286;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_10320) )
    return GAL_SPECLINES_S_II_10320;
  else if( !strcmp(name, GAL_SPECLINES_NAME_S_II_10336) )
    return GAL_SPECLINES_S_II_10336;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Fe_XIII) )
    return GAL_SPECLINES_Fe_XIII;
  else if( !strcmp(name, GAL_SPECLINES_NAME_He_I_10830) )
    return GAL_SPECLINES_He_I_10830;
  else if( !strcmp(name, GAL_SPECLINES_NAME_Pa_gamma) )
    return GAL_SPECLINES_Pa_gamma;

  /* Limits */
  else if( !strcmp(name, GAL_SPECLINES_NAME_LIMIT_LYMAN) )
    return GAL_SPECLINES_LIMIT_LYMAN;
  else if( !strcmp(name, GAL_SPECLINES_NAME_LIMIT_BALMER) )
    return GAL_SPECLINES_LIMIT_BALMER;
  else if( !strcmp(name, GAL_SPECLINES_NAME_LIMIT_PASCHEN) )
    return GAL_SPECLINES_LIMIT_PASCHEN;

  /* Invalid. */
  else return GAL_SPECLINES_INVALID;
  return GAL_SPECLINES_INVALID;
}





/* Return the wavelength (in Angstroms) of given line. */
double
gal_speclines_line_angstrom(int linecode)
{
  switch(linecode)
    {
    case GAL_SPECLINES_Ne_VIII_770:    return GAL_SPECLINES_ANGSTROM_Ne_VIII_770;
    case GAL_SPECLINES_Ne_VIII_780:    return GAL_SPECLINES_ANGSTROM_Ne_VIII_780;
    case GAL_SPECLINES_Ly_epsilon:     return GAL_SPECLINES_ANGSTROM_Ly_epsilon;
    case GAL_SPECLINES_Ly_delta:       return GAL_SPECLINES_ANGSTROM_Ly_delta;
    case GAL_SPECLINES_Ly_gamma:       return GAL_SPECLINES_ANGSTROM_Ly_gamma;
    case GAL_SPECLINES_C_III_977:      return GAL_SPECLINES_ANGSTROM_C_III_977;
    case GAL_SPECLINES_N_III_989:      return GAL_SPECLINES_ANGSTROM_N_III_989;
    case GAL_SPECLINES_N_III_991_51:   return GAL_SPECLINES_ANGSTROM_N_III_991_51;
    case GAL_SPECLINES_N_III_991_57:   return GAL_SPECLINES_ANGSTROM_N_III_991_57;
    case GAL_SPECLINES_Ly_beta:        return GAL_SPECLINES_ANGSTROM_Ly_beta;
    case GAL_SPECLINES_O_VI_1031:      return GAL_SPECLINES_ANGSTROM_O_VI_1031;
    case GAL_SPECLINES_O_VI_1037:      return GAL_SPECLINES_ANGSTROM_O_VI_1037;
    case GAL_SPECLINES_Ar_I_1066:      return GAL_SPECLINES_ANGSTROM_Ar_I_1066;
    case GAL_SPECLINES_Ly_alpha:       return GAL_SPECLINES_ANGSTROM_Ly_alpha;
    case GAL_SPECLINES_N_V_1238:       return GAL_SPECLINES_ANGSTROM_N_V_1238;
    case GAL_SPECLINES_N_V_1242:       return GAL_SPECLINES_ANGSTROM_N_V_1242;
    case GAL_SPECLINES_Si_II_1260:     return GAL_SPECLINES_ANGSTROM_Si_II_1260;
    case GAL_SPECLINES_Si_II_1264:     return GAL_SPECLINES_ANGSTROM_Si_II_1264;
    case GAL_SPECLINES_O_I_1302:       return GAL_SPECLINES_ANGSTROM_O_I_1302;
    case GAL_SPECLINES_C_II_1334:      return GAL_SPECLINES_ANGSTROM_C_II_1334;
    case GAL_SPECLINES_C_II_1335:      return GAL_SPECLINES_ANGSTROM_C_II_1335;
    case GAL_SPECLINES_Si_IV_1393:     return GAL_SPECLINES_ANGSTROM_Si_IV_1393;
    case GAL_SPECLINES_O_IV_1397:      return GAL_SPECLINES_ANGSTROM_O_IV_1397;
    case GAL_SPECLINES_O_IV_1399:      return GAL_SPECLINES_ANGSTROM_O_IV_1399;
    case GAL_SPECLINES_Si_IV_1402:     return GAL_SPECLINES_ANGSTROM_Si_IV_1402;
    case GAL_SPECLINES_N_IV_1486:      return GAL_SPECLINES_ANGSTROM_N_IV_1486;
    case GAL_SPECLINES_C_IV_1548:      return GAL_SPECLINES_ANGSTROM_C_IV_1548;
    case GAL_SPECLINES_C_IV_1550:      return GAL_SPECLINES_ANGSTROM_C_IV_1550;
    case GAL_SPECLINES_He_II_1640:     return GAL_SPECLINES_ANGSTROM_He_II_1640;
    case GAL_SPECLINES_O_III_1660:     return GAL_SPECLINES_ANGSTROM_O_III_1660;
    case GAL_SPECLINES_O_III_1666:     return GAL_SPECLINES_ANGSTROM_O_III_1666;
    case GAL_SPECLINES_N_III_1746:     return GAL_SPECLINES_ANGSTROM_N_III_1746;
    case GAL_SPECLINES_N_III_1748:     return GAL_SPECLINES_ANGSTROM_N_III_1748;
    case GAL_SPECLINES_Al_III_1854:    return GAL_SPECLINES_ANGSTROM_Al_III_1854;
    case GAL_SPECLINES_Al_III_1862:    return GAL_SPECLINES_ANGSTROM_Al_III_1862;
    case GAL_SPECLINES_Si_III:         return GAL_SPECLINES_ANGSTROM_Si_III;
    case GAL_SPECLINES_C_III_1908:     return GAL_SPECLINES_ANGSTROM_C_III_1908;
    case GAL_SPECLINES_N_II_2142:      return GAL_SPECLINES_ANGSTROM_N_II_2142;
    case GAL_SPECLINES_O_III_2320:     return GAL_SPECLINES_ANGSTROM_O_III_2320;
    case GAL_SPECLINES_C_II_2323:      return GAL_SPECLINES_ANGSTROM_C_II_2323;
    case GAL_SPECLINES_C_II_2324:      return GAL_SPECLINES_ANGSTROM_C_II_2324;
    case GAL_SPECLINES_Fe_XI_2648:     return GAL_SPECLINES_ANGSTROM_Fe_XI_2648;
    case GAL_SPECLINES_He_II_2733:     return GAL_SPECLINES_ANGSTROM_He_II_2733;
    case GAL_SPECLINES_Mg_V_2782:      return GAL_SPECLINES_ANGSTROM_Mg_V_2782;
    case GAL_SPECLINES_Mg_II_2795:     return GAL_SPECLINES_ANGSTROM_Mg_II_2795;
    case GAL_SPECLINES_Mg_II_2802:     return GAL_SPECLINES_ANGSTROM_Mg_II_2802;
    case GAL_SPECLINES_Fe_IV_2829:     return GAL_SPECLINES_ANGSTROM_Fe_IV_2829;
    case GAL_SPECLINES_Fe_IV_2835:     return GAL_SPECLINES_ANGSTROM_Fe_IV_2835;
    case GAL_SPECLINES_Ar_IV_2853:     return GAL_SPECLINES_ANGSTROM_Ar_IV_2853;
    case GAL_SPECLINES_Ar_IV_2868:     return GAL_SPECLINES_ANGSTROM_Ar_IV_2868;
    case GAL_SPECLINES_Mg_V_2928:      return GAL_SPECLINES_ANGSTROM_Mg_V_2928;
    case GAL_SPECLINES_He_I_2945:      return GAL_SPECLINES_ANGSTROM_He_I_2945;
    case GAL_SPECLINES_O_III_3132:     return GAL_SPECLINES_ANGSTROM_O_III_3132;
    case GAL_SPECLINES_He_I_3187:      return GAL_SPECLINES_ANGSTROM_He_I_3187;
    case GAL_SPECLINES_He_II_3203:     return GAL_SPECLINES_ANGSTROM_He_II_3203;
    case GAL_SPECLINES_O_III_3312:     return GAL_SPECLINES_ANGSTROM_O_III_3312;
    case GAL_SPECLINES_Ne_V_3345:      return GAL_SPECLINES_ANGSTROM_Ne_V_3345;
    case GAL_SPECLINES_Ne_V_3425:      return GAL_SPECLINES_ANGSTROM_Ne_V_3425;
    case GAL_SPECLINES_O_III_3444:     return GAL_SPECLINES_ANGSTROM_O_III_3444;
    case GAL_SPECLINES_N_I_3466_4:     return GAL_SPECLINES_ANGSTROM_N_I_3466_4;
    case GAL_SPECLINES_N_I_3466_5:     return GAL_SPECLINES_ANGSTROM_N_I_3466_5;
    case GAL_SPECLINES_He_I_3487:      return GAL_SPECLINES_ANGSTROM_He_I_3487;
    case GAL_SPECLINES_Fe_VII_3586:    return GAL_SPECLINES_ANGSTROM_Fe_VII_3586;
    case GAL_SPECLINES_Fe_VI_3662:     return GAL_SPECLINES_ANGSTROM_Fe_VI_3662;
    case GAL_SPECLINES_H_19:           return GAL_SPECLINES_ANGSTROM_H_19;
    case GAL_SPECLINES_H_18:           return GAL_SPECLINES_ANGSTROM_H_18;
    case GAL_SPECLINES_H_17:           return GAL_SPECLINES_ANGSTROM_H_17;
    case GAL_SPECLINES_H_16:           return GAL_SPECLINES_ANGSTROM_H_16;
    case GAL_SPECLINES_H_15:           return GAL_SPECLINES_ANGSTROM_H_15;
    case GAL_SPECLINES_H_14:           return GAL_SPECLINES_ANGSTROM_H_14;
    case GAL_SPECLINES_O_II_3726:      return GAL_SPECLINES_ANGSTROM_O_II_3726;
    case GAL_SPECLINES_O_II_3728:      return GAL_SPECLINES_ANGSTROM_O_II_3728;
    case GAL_SPECLINES_H_13:           return GAL_SPECLINES_ANGSTROM_H_13;
    case GAL_SPECLINES_H_12:           return GAL_SPECLINES_ANGSTROM_H_12;
    case GAL_SPECLINES_Fe_VII_3758:    return GAL_SPECLINES_ANGSTROM_Fe_VII_3758;
    case GAL_SPECLINES_H_11:           return GAL_SPECLINES_ANGSTROM_H_11;
    case GAL_SPECLINES_H_10:           return GAL_SPECLINES_ANGSTROM_H_10;
    case GAL_SPECLINES_H_9:            return GAL_SPECLINES_ANGSTROM_H_9;
    case GAL_SPECLINES_Fe_V_3839:      return GAL_SPECLINES_ANGSTROM_Fe_V_3839;
    case GAL_SPECLINES_Ne_III_3868:    return GAL_SPECLINES_ANGSTROM_Ne_III_3868;
    case GAL_SPECLINES_He_I_3888:      return GAL_SPECLINES_ANGSTROM_He_I_3888;
    case GAL_SPECLINES_H_8:            return GAL_SPECLINES_ANGSTROM_H_8;
    case GAL_SPECLINES_Fe_V_3891:      return GAL_SPECLINES_ANGSTROM_Fe_V_3891;
    case GAL_SPECLINES_Fe_V_3911:      return GAL_SPECLINES_ANGSTROM_Fe_V_3911;
    case GAL_SPECLINES_Ne_III_3967:    return GAL_SPECLINES_ANGSTROM_Ne_III_3967;
    case GAL_SPECLINES_H_epsilon:      return GAL_SPECLINES_ANGSTROM_H_epsilon;
    case GAL_SPECLINES_He_I_4026:      return GAL_SPECLINES_ANGSTROM_He_I_4026;
    case GAL_SPECLINES_S_II_4068:      return GAL_SPECLINES_ANGSTROM_S_II_4068;
    case GAL_SPECLINES_Fe_V_4071:      return GAL_SPECLINES_ANGSTROM_Fe_V_4071;
    case GAL_SPECLINES_S_II_4076:      return GAL_SPECLINES_ANGSTROM_S_II_4076;
    case GAL_SPECLINES_H_delta:        return GAL_SPECLINES_ANGSTROM_H_delta;
    case GAL_SPECLINES_He_I_4143:      return GAL_SPECLINES_ANGSTROM_He_I_4143;
    case GAL_SPECLINES_Fe_II_4178:     return GAL_SPECLINES_ANGSTROM_Fe_II_4178;
    case GAL_SPECLINES_Fe_V_4180:      return GAL_SPECLINES_ANGSTROM_Fe_V_4180;
    case GAL_SPECLINES_Fe_II_4233:     return GAL_SPECLINES_ANGSTROM_Fe_II_4233;
    case GAL_SPECLINES_Fe_V_4227:      return GAL_SPECLINES_ANGSTROM_Fe_V_4227;
    case GAL_SPECLINES_Fe_II_4287:     return GAL_SPECLINES_ANGSTROM_Fe_II_4287;
    case GAL_SPECLINES_Fe_II_4304:     return GAL_SPECLINES_ANGSTROM_Fe_II_4304;
    case GAL_SPECLINES_O_II_4317:      return GAL_SPECLINES_ANGSTROM_O_II_4317;
    case GAL_SPECLINES_H_gamma:        return GAL_SPECLINES_ANGSTROM_H_gamma;
    case GAL_SPECLINES_O_III_4363:     return GAL_SPECLINES_ANGSTROM_O_III_4363;
    case GAL_SPECLINES_Ar_XIV:         return GAL_SPECLINES_ANGSTROM_Ar_XIV;
    case GAL_SPECLINES_O_II_4414:      return GAL_SPECLINES_ANGSTROM_O_II_4414;
    case GAL_SPECLINES_Fe_II_4416:     return GAL_SPECLINES_ANGSTROM_Fe_II_4416;
    case GAL_SPECLINES_Fe_II_4452:     return GAL_SPECLINES_ANGSTROM_Fe_II_4452;
    case GAL_SPECLINES_He_I_4471:      return GAL_SPECLINES_ANGSTROM_He_I_4471;
    case GAL_SPECLINES_Fe_II_4489:     return GAL_SPECLINES_ANGSTROM_Fe_II_4489;
    case GAL_SPECLINES_Fe_II_4491:     return GAL_SPECLINES_ANGSTROM_Fe_II_4491;
    case GAL_SPECLINES_N_III_4510:     return GAL_SPECLINES_ANGSTROM_N_III_4510;
    case GAL_SPECLINES_Fe_II_4522:     return GAL_SPECLINES_ANGSTROM_Fe_II_4522;
    case GAL_SPECLINES_Fe_II_4555:     return GAL_SPECLINES_ANGSTROM_Fe_II_4555;
    case GAL_SPECLINES_Fe_II_4582:     return GAL_SPECLINES_ANGSTROM_Fe_II_4582;
    case GAL_SPECLINES_Fe_II_4583:     return GAL_SPECLINES_ANGSTROM_Fe_II_4583;
    case GAL_SPECLINES_Fe_II_4629:     return GAL_SPECLINES_ANGSTROM_Fe_II_4629;
    case GAL_SPECLINES_N_III_4634:     return GAL_SPECLINES_ANGSTROM_N_III_4634;
    case GAL_SPECLINES_N_III_4640:     return GAL_SPECLINES_ANGSTROM_N_III_4640;
    case GAL_SPECLINES_N_III_4641:     return GAL_SPECLINES_ANGSTROM_N_III_4641;
    case GAL_SPECLINES_C_III_4647:     return GAL_SPECLINES_ANGSTROM_C_III_4647;
    case GAL_SPECLINES_C_III_4650:     return GAL_SPECLINES_ANGSTROM_C_III_4650;
    case GAL_SPECLINES_C_III_5651:     return GAL_SPECLINES_ANGSTROM_C_III_5651;
    case GAL_SPECLINES_Fe_III_4658:    return GAL_SPECLINES_ANGSTROM_Fe_III_4658;
    case GAL_SPECLINES_He_II_4685:     return GAL_SPECLINES_ANGSTROM_He_II_4685;
    case GAL_SPECLINES_Ar_IV_4711:     return GAL_SPECLINES_ANGSTROM_Ar_IV_4711;
    case GAL_SPECLINES_Ar_IV_4740:     return GAL_SPECLINES_ANGSTROM_Ar_IV_4740;
    case GAL_SPECLINES_H_beta:         return GAL_SPECLINES_ANGSTROM_H_beta;
    case GAL_SPECLINES_Fe_VII_4893:    return GAL_SPECLINES_ANGSTROM_Fe_VII_4893;
    case GAL_SPECLINES_Fe_IV_4903:     return GAL_SPECLINES_ANGSTROM_Fe_IV_4903;
    case GAL_SPECLINES_Fe_II_4923:     return GAL_SPECLINES_ANGSTROM_Fe_II_4923;
    case GAL_SPECLINES_O_III_4958:     return GAL_SPECLINES_ANGSTROM_O_III_4958;
    case GAL_SPECLINES_O_III_5006:     return GAL_SPECLINES_ANGSTROM_O_III_5006;
    case GAL_SPECLINES_Fe_II_5018:     return GAL_SPECLINES_ANGSTROM_Fe_II_5018;
    case GAL_SPECLINES_Fe_III_5084:    return GAL_SPECLINES_ANGSTROM_Fe_III_5084;
    case GAL_SPECLINES_Fe_VI_5145:     return GAL_SPECLINES_ANGSTROM_Fe_VI_5145;
    case GAL_SPECLINES_Fe_VII_5158:    return GAL_SPECLINES_ANGSTROM_Fe_VII_5158;
    case GAL_SPECLINES_Fe_II_5169:     return GAL_SPECLINES_ANGSTROM_Fe_II_5169;
    case GAL_SPECLINES_Fe_VI_5176:     return GAL_SPECLINES_ANGSTROM_Fe_VI_5176;
    case GAL_SPECLINES_Fe_II_5197:     return GAL_SPECLINES_ANGSTROM_Fe_II_5197;
    case GAL_SPECLINES_N_I_5200:       return GAL_SPECLINES_ANGSTROM_N_I_5200;
    case GAL_SPECLINES_Fe_II_5234:     return GAL_SPECLINES_ANGSTROM_Fe_II_5234;
    case GAL_SPECLINES_Fe_IV_5236:     return GAL_SPECLINES_ANGSTROM_Fe_IV_5236;
    case GAL_SPECLINES_Fe_III_5270:    return GAL_SPECLINES_ANGSTROM_Fe_III_5270;
    case GAL_SPECLINES_Fe_II_5276:     return GAL_SPECLINES_ANGSTROM_Fe_II_5276;
    case GAL_SPECLINES_Fe_VII_5276:    return GAL_SPECLINES_ANGSTROM_Fe_VII_5276;
    case GAL_SPECLINES_Fe_XIV:         return GAL_SPECLINES_ANGSTROM_Fe_XIV;
    case GAL_SPECLINES_Ca_V:           return GAL_SPECLINES_ANGSTROM_Ca_V;
    case GAL_SPECLINES_Fe_II_5316_6:   return GAL_SPECLINES_ANGSTROM_Fe_II_5316_6;
    case GAL_SPECLINES_Fe_II_5316_7:   return GAL_SPECLINES_ANGSTROM_Fe_II_5316_7;
    case GAL_SPECLINES_Fe_VI_5335:     return GAL_SPECLINES_ANGSTROM_Fe_VI_5335;
    case GAL_SPECLINES_Fe_VI_5424:     return GAL_SPECLINES_ANGSTROM_Fe_VI_5424;
    case GAL_SPECLINES_Cl_III_5517:    return GAL_SPECLINES_ANGSTROM_Cl_III_5517;
    case GAL_SPECLINES_Cl_III_5537:    return GAL_SPECLINES_ANGSTROM_Cl_III_5537;
    case GAL_SPECLINES_Fe_VI_5637:     return GAL_SPECLINES_ANGSTROM_Fe_VI_5637;
    case GAL_SPECLINES_Fe_VI_5677:     return GAL_SPECLINES_ANGSTROM_Fe_VI_5677;
    case GAL_SPECLINES_C_III_5697:     return GAL_SPECLINES_ANGSTROM_C_III_5697;
    case GAL_SPECLINES_Fe_VII_5720:    return GAL_SPECLINES_ANGSTROM_Fe_VII_5720;
    case GAL_SPECLINES_N_II_5754:      return GAL_SPECLINES_ANGSTROM_N_II_5754;
    case GAL_SPECLINES_C_IV_5801:      return GAL_SPECLINES_ANGSTROM_C_IV_5801;
    case GAL_SPECLINES_C_IV_5811:      return GAL_SPECLINES_ANGSTROM_C_IV_5811;
    case GAL_SPECLINES_He_I_5875:      return GAL_SPECLINES_ANGSTROM_He_I_5875;
    case GAL_SPECLINES_O_I_6046:       return GAL_SPECLINES_ANGSTROM_O_I_6046;
    case GAL_SPECLINES_Fe_VII_6087:    return GAL_SPECLINES_ANGSTROM_Fe_VII_6087;
    case GAL_SPECLINES_O_I_6300:       return GAL_SPECLINES_ANGSTROM_O_I_6300;
    case GAL_SPECLINES_S_III_6312:     return GAL_SPECLINES_ANGSTROM_S_III_6312;
    case GAL_SPECLINES_Si_II_6347:     return GAL_SPECLINES_ANGSTROM_Si_II_6347;
    case GAL_SPECLINES_O_I_6363:       return GAL_SPECLINES_ANGSTROM_O_I_6363;
    case GAL_SPECLINES_Fe_II_6369:     return GAL_SPECLINES_ANGSTROM_Fe_II_6369;
    case GAL_SPECLINES_Fe_X:           return GAL_SPECLINES_ANGSTROM_Fe_X;
    case GAL_SPECLINES_Fe_II_6516:     return GAL_SPECLINES_ANGSTROM_Fe_II_6516;
    case GAL_SPECLINES_N_II_6548:      return GAL_SPECLINES_ANGSTROM_N_II_6548;
    case GAL_SPECLINES_H_alpha:        return GAL_SPECLINES_ANGSTROM_H_alpha;
    case GAL_SPECLINES_N_II_6583:      return GAL_SPECLINES_ANGSTROM_N_II_6583;
    case GAL_SPECLINES_S_II_6716:      return GAL_SPECLINES_ANGSTROM_S_II_6716;
    case GAL_SPECLINES_S_II_6730:      return GAL_SPECLINES_ANGSTROM_S_II_6730;
    case GAL_SPECLINES_O_I_7002:       return GAL_SPECLINES_ANGSTROM_O_I_7002;
    case GAL_SPECLINES_Ar_V:           return GAL_SPECLINES_ANGSTROM_Ar_V;
    case GAL_SPECLINES_He_I_7065:      return GAL_SPECLINES_ANGSTROM_He_I_7065;
    case GAL_SPECLINES_Ar_III_7135:    return GAL_SPECLINES_ANGSTROM_Ar_III_7135;
    case GAL_SPECLINES_Fe_II_7155:     return GAL_SPECLINES_ANGSTROM_Fe_II_7155;
    case GAL_SPECLINES_Ar_IV_7170:     return GAL_SPECLINES_ANGSTROM_Ar_IV_7170;
    case GAL_SPECLINES_Fe_II_7172:     return GAL_SPECLINES_ANGSTROM_Fe_II_7172;
    case GAL_SPECLINES_C_II_7236:      return GAL_SPECLINES_ANGSTROM_C_II_7236;
    case GAL_SPECLINES_Ar_IV_7237:     return GAL_SPECLINES_ANGSTROM_Ar_IV_7237;
    case GAL_SPECLINES_O_I_7254:       return GAL_SPECLINES_ANGSTROM_O_I_7254;
    case GAL_SPECLINES_Ar_IV_7262:     return GAL_SPECLINES_ANGSTROM_Ar_IV_7262;
    case GAL_SPECLINES_He_I_7281:      return GAL_SPECLINES_ANGSTROM_He_I_7281;
    case GAL_SPECLINES_O_II_7319:      return GAL_SPECLINES_ANGSTROM_O_II_7319;
    case GAL_SPECLINES_O_II_7330:      return GAL_SPECLINES_ANGSTROM_O_II_7330;
    case GAL_SPECLINES_Ni_II_7377:     return GAL_SPECLINES_ANGSTROM_Ni_II_7377;
    case GAL_SPECLINES_Ni_II_7411:     return GAL_SPECLINES_ANGSTROM_Ni_II_7411;
    case GAL_SPECLINES_Fe_II_7452:     return GAL_SPECLINES_ANGSTROM_Fe_II_7452;
    case GAL_SPECLINES_N_I_7468:       return GAL_SPECLINES_ANGSTROM_N_I_7468;
    case GAL_SPECLINES_S_XII:          return GAL_SPECLINES_ANGSTROM_S_XII;
    case GAL_SPECLINES_Ar_III_7751:    return GAL_SPECLINES_ANGSTROM_Ar_III_7751;
    case GAL_SPECLINES_He_I_7816:      return GAL_SPECLINES_ANGSTROM_He_I_7816;
    case GAL_SPECLINES_Ar_I_7868:      return GAL_SPECLINES_ANGSTROM_Ar_I_7868;
    case GAL_SPECLINES_Ni_III:         return GAL_SPECLINES_ANGSTROM_Ni_III;
    case GAL_SPECLINES_Fe_XI_7891:     return GAL_SPECLINES_ANGSTROM_Fe_XI_7891;
    case GAL_SPECLINES_He_II_8236:     return GAL_SPECLINES_ANGSTROM_He_II_8236;
    case GAL_SPECLINES_Pa_20:          return GAL_SPECLINES_ANGSTROM_Pa_20;
    case GAL_SPECLINES_Pa_19:          return GAL_SPECLINES_ANGSTROM_Pa_19;
    case GAL_SPECLINES_Pa_18:          return GAL_SPECLINES_ANGSTROM_Pa_18;
    case GAL_SPECLINES_O_I_8446:       return GAL_SPECLINES_ANGSTROM_O_I_8446;
    case GAL_SPECLINES_Pa_17:          return GAL_SPECLINES_ANGSTROM_Pa_17;
    case GAL_SPECLINES_Ca_II_8498:     return GAL_SPECLINES_ANGSTROM_Ca_II_8498;
    case GAL_SPECLINES_Pa_16:          return GAL_SPECLINES_ANGSTROM_Pa_16;
    case GAL_SPECLINES_Ca_II_8542:     return GAL_SPECLINES_ANGSTROM_Ca_II_8542;
    case GAL_SPECLINES_Pa_15:          return GAL_SPECLINES_ANGSTROM_Pa_15;
    case GAL_SPECLINES_Cl_II:          return GAL_SPECLINES_ANGSTROM_Cl_II;
    case GAL_SPECLINES_Pa_14:          return GAL_SPECLINES_ANGSTROM_Pa_14;
    case GAL_SPECLINES_Fe_II_8616:     return GAL_SPECLINES_ANGSTROM_Fe_II_8616;
    case GAL_SPECLINES_Ca_II_8662:     return GAL_SPECLINES_ANGSTROM_Ca_II_8662;
    case GAL_SPECLINES_Pa_13:          return GAL_SPECLINES_ANGSTROM_Pa_13;
    case GAL_SPECLINES_N_I_8680:       return GAL_SPECLINES_ANGSTROM_N_I_8680;
    case GAL_SPECLINES_N_I_8703:       return GAL_SPECLINES_ANGSTROM_N_I_8703;
    case GAL_SPECLINES_N_I_8711:       return GAL_SPECLINES_ANGSTROM_N_I_8711;
    case GAL_SPECLINES_Pa_12:          return GAL_SPECLINES_ANGSTROM_Pa_12;
    case GAL_SPECLINES_Pa_11:          return GAL_SPECLINES_ANGSTROM_Pa_11;
    case GAL_SPECLINES_Fe_II_8891:     return GAL_SPECLINES_ANGSTROM_Fe_II_8891;
    case GAL_SPECLINES_Pa_10:          return GAL_SPECLINES_ANGSTROM_Pa_10;
    case GAL_SPECLINES_S_III_9068:     return GAL_SPECLINES_ANGSTROM_S_III_9068;
    case GAL_SPECLINES_Pa_9:           return GAL_SPECLINES_ANGSTROM_Pa_9;
    case GAL_SPECLINES_S_III_9531:     return GAL_SPECLINES_ANGSTROM_S_III_9531;
    case GAL_SPECLINES_Pa_epsilon:     return GAL_SPECLINES_ANGSTROM_Pa_epsilon;
    case GAL_SPECLINES_C_I_9824:       return GAL_SPECLINES_ANGSTROM_C_I_9824;
    case GAL_SPECLINES_C_I_9850:       return GAL_SPECLINES_ANGSTROM_C_I_9850;
    case GAL_SPECLINES_S_VIII:         return GAL_SPECLINES_ANGSTROM_S_VIII;
    case GAL_SPECLINES_He_I_10027:     return GAL_SPECLINES_ANGSTROM_He_I_10027;
    case GAL_SPECLINES_He_I_10031:     return GAL_SPECLINES_ANGSTROM_He_I_10031;
    case GAL_SPECLINES_Pa_delta:       return GAL_SPECLINES_ANGSTROM_Pa_delta;
    case GAL_SPECLINES_S_II_10286:     return GAL_SPECLINES_ANGSTROM_S_II_10286;
    case GAL_SPECLINES_S_II_10320:     return GAL_SPECLINES_ANGSTROM_S_II_10320;
    case GAL_SPECLINES_S_II_10336:     return GAL_SPECLINES_ANGSTROM_S_II_10336;
    case GAL_SPECLINES_Fe_XIII:        return GAL_SPECLINES_ANGSTROM_Fe_XIII;
    case GAL_SPECLINES_He_I_10830:     return GAL_SPECLINES_ANGSTROM_He_I_10830;
    case GAL_SPECLINES_Pa_gamma:       return GAL_SPECLINES_ANGSTROM_Pa_gamma;

    /* Limits */
    case GAL_SPECLINES_LIMIT_LYMAN:    return GAL_SPECLINES_ANGSTROM_LIMIT_LYMAN;
    case GAL_SPECLINES_LIMIT_BALMER:   return GAL_SPECLINES_ANGSTROM_LIMIT_BALMER;
    case GAL_SPECLINES_LIMIT_PASCHEN:  return GAL_SPECLINES_ANGSTROM_LIMIT_PASCHEN;

    default:
      error(EXIT_FAILURE, 0, "%s: '%d' not recognized line identifier",
            __func__, linecode);
    }
  return NAN;
}




















/*********************************************************************/
/*************             Redshifted lines            ***************/
/*********************************************************************/
double
gal_speclines_line_redshift(double obsline, double restline)
{
  return (obsline/restline)-1;
}





double
gal_speclines_line_redshift_code(double obsline, int linecode)
{
  double restline=gal_speclines_line_angstrom(linecode);
  return (obsline/restline)-1;
}
