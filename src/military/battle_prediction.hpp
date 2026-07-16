#pragma once
namespace predictions {

const float Cw[78] =
{
	
	0.5175060256127391f, 
	1.903181928950741f, 
	-1.385675903338183f, 
	0.09640766786251354f, 
	-0.27161173238942193f, 
	0.07365004758200004f, 
	0.8272447118113418f, 
	-0.41280138528057964f, 
	0.11961991265596164f, 
	-0.24974947557786847f, 
	0.11034626540490038f, 
	0.007571467583437612f, 
	1.5823030491228336f, 
	0.3208788798278998f, 
	0.5254414169820306f, 
	0.6254797123707418f, 
	0.34606280326408295f, 
	1.5238018160320224f, 
	1.005279650227793f, 
	0.9432093104396955f, 
	0.8370108097422441f, 
	0.6562715460667097f, 
	0.5792922694173566f, 
	-1.7065547831658499f, 
	-0.42903374911945824f, 
	-0.8970914447602417f, 
	-0.27241275568212264f, 
	-0.6965571042206838f, 
	-1.418081035508456f, 
	-0.8235893977836958f, 
	-1.0867602853200766f, 
	-0.5459252806616984f, 
	-0.5717208018339193f, 
	0.007731904286899669f, 
	0.06897211231639284f, 
	-0.030551373724166452f, 
	0.0019602931812536055f, 
	0.012382559057957512f, 
	-0.0009917049078075435f, 
	-0.04538813711974315f, 
	-0.019570155849862193f, 
	-0.07384609407497388f, 
	0.22493418392171452f, 
	-0.23780317418095842f, 
	0.11056020358570609f, 
	-0.11076785610887742f, 
	0.22402914899193835f, 
	-0.10879135307713546f, 
	-0.04860623184810476f, 
	-0.14599632509903815f, 
	-0.08373451715562388f, 
	-0.09865011936197765f, 
	-0.1567938153095523f, 
	0.07235707533723151f, 
	-0.04247400745136163f, 
	-0.06104813310385584f, 
	-0.1861647108518383f, 
	-0.09710950904839225f, 
	-0.05884356576175635f, 
	0.13727629230875382f, 
	-0.0019992489024051024f, 
	-0.06369161120704414f, 
	0.09206118787490954f, 
	-0.08303785424072896f, 
	0.11432115562851904f, 
	-0.1343537497534375f, 
	0.2436713414636248f, 
	0.2772165318288174f, 
	-0.0025607862668593447f, 
	-0.0176303745422286f, 
	0.179438906732208f, 
	0.1557558363018889f, 
	0.006911400273768966f, 
	-0.05855082398417053f, 
	-0.025752322270364583f, 
	-0.056819731334453824f, 
	-0.19890028382983338f, 
	-0.25408723808405337f
};
float battle_win_score(
	float attacker_str,
	float defender_str,
	float dig_in,
	float crossing,
	float terrain,
	float tactic_attacker,
	float tactic_defender,
	float attacker_general_stat,
	float defender_general_stat,
	float attacker_general_prestige,
	float defender_general_prestige
) {
	float score = 0.f
	+ Cw[1] * attacker_str
	+ Cw[2] * defender_str
	+ Cw[3] * dig_in
	+ Cw[4] * crossing
	+ Cw[5] * terrain
	+ Cw[6] * tactic_attacker
	+ Cw[7] * tactic_defender
	+ Cw[8] * attacker_general_stat
	+ Cw[9] * defender_general_stat
	+ Cw[10] * attacker_general_prestige
	+ Cw[11] * defender_general_prestige
	+ Cw[12] * attacker_str * attacker_str
	+ Cw[13] * attacker_str * defender_str
	+ Cw[14] * attacker_str * dig_in
	+ Cw[15] * attacker_str * crossing
	+ Cw[16] * attacker_str * terrain
	+ Cw[17] * attacker_str * tactic_attacker
	+ Cw[18] * attacker_str * tactic_defender
	+ Cw[19] * attacker_str * attacker_general_stat
	+ Cw[20] * attacker_str * defender_general_stat
	+ Cw[21] * attacker_str * attacker_general_prestige
	+ Cw[22] * attacker_str * defender_general_prestige
	+ Cw[23] * defender_str * defender_str
	+ Cw[24] * defender_str * dig_in
	+ Cw[25] * defender_str * crossing
	+ Cw[26] * defender_str * terrain
	+ Cw[27] * defender_str * tactic_attacker
	+ Cw[28] * defender_str * tactic_defender
	+ Cw[29] * defender_str * attacker_general_stat
	+ Cw[30] * defender_str * defender_general_stat
	+ Cw[31] * defender_str * attacker_general_prestige
	+ Cw[32] * defender_str * defender_general_prestige
	+ Cw[33] * dig_in * dig_in
	+ Cw[34] * dig_in * crossing
	+ Cw[35] * dig_in * terrain
	+ Cw[36] * dig_in * tactic_attacker
	+ Cw[37] * dig_in * tactic_defender
	+ Cw[38] * dig_in * attacker_general_stat
	+ Cw[39] * dig_in * defender_general_stat
	+ Cw[40] * dig_in * attacker_general_prestige
	+ Cw[41] * dig_in * defender_general_prestige
	+ Cw[42] * crossing * crossing
	+ Cw[43] * crossing * terrain
	+ Cw[44] * crossing * tactic_attacker
	+ Cw[45] * crossing * tactic_defender
	+ Cw[46] * crossing * attacker_general_stat
	+ Cw[47] * crossing * defender_general_stat
	+ Cw[48] * crossing * attacker_general_prestige
	+ Cw[49] * crossing * defender_general_prestige
	+ Cw[50] * terrain * terrain
	+ Cw[51] * terrain * tactic_attacker
	+ Cw[52] * terrain * tactic_defender
	+ Cw[53] * terrain * attacker_general_stat
	+ Cw[54] * terrain * defender_general_stat
	+ Cw[55] * terrain * attacker_general_prestige
	+ Cw[56] * terrain * defender_general_prestige
	+ Cw[57] * tactic_attacker * tactic_attacker
	+ Cw[58] * tactic_attacker * tactic_defender
	+ Cw[59] * tactic_attacker * attacker_general_stat
	+ Cw[60] * tactic_attacker * defender_general_stat
	+ Cw[61] * tactic_attacker * attacker_general_prestige
	+ Cw[62] * tactic_attacker * defender_general_prestige
	+ Cw[63] * tactic_defender * tactic_defender
	+ Cw[64] * tactic_defender * attacker_general_stat
	+ Cw[65] * tactic_defender * defender_general_stat
	+ Cw[66] * tactic_defender * attacker_general_prestige
	+ Cw[67] * tactic_defender * defender_general_prestige
	+ Cw[68] * attacker_general_stat * attacker_general_stat
	+ Cw[69] * attacker_general_stat * defender_general_stat
	+ Cw[70] * attacker_general_stat * attacker_general_prestige
	+ Cw[71] * attacker_general_stat * defender_general_prestige
	+ Cw[72] * defender_general_stat * defender_general_stat
	+ Cw[73] * defender_general_stat * attacker_general_prestige
	+ Cw[74] * defender_general_stat * defender_general_prestige
	+ Cw[75] * attacker_general_prestige * attacker_general_prestige
	+ Cw[76] * attacker_general_prestige * defender_general_prestige
	+ Cw[77] * defender_general_prestige * defender_general_prestige;
	return score;
}

// https://scicomp.stackexchange.com/a/42187
constexpr float exp_product_approx16(float x){
	x = 1 + x / 16;
	const auto a = x * x;
	const auto b = a * a;
	const auto c = b * b;
	const auto d = c * c;
	return d;
}
inline float log_w_approx_exp_no_overflow16_clamped(float x) {
	if(x >= 16) [[unlikely]] {
		return 1;
	}
	return 1 / (1 + exp_product_approx16(-x));
}

inline float battle_win_probability(
	float attacker_str,
	float defender_str,
	float dig_in,
	float crossing,
	float terrain,
	float tactic_attacker,
	float tactic_defender,
	float attacker_general_stat,
	float defender_general_stat,
	float attacker_general_prestige,
	float defender_general_prestige
) {
	auto score = battle_win_score(attacker_str / (attacker_str + defender_str), defender_str / (attacker_str + defender_str), dig_in, crossing, terrain, tactic_attacker, tactic_defender, attacker_general_stat, defender_general_stat, attacker_general_prestige, defender_general_prestige);
	auto proba = log_w_approx_exp_no_overflow16_clamped(score);
	return proba;
}

}
