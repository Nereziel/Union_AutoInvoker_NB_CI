META
{
    Parser = Game;
    MergeMode = 1;
    Engine = G2A;
};

// Requires the new Union_AutoInvoker_NB.dll.
// 1% of maximum mana each second, minimum 1 point/second.
// Applies only to characters with magic circle >= 1.
// Does NOT require full HP.
var int NB_ManaDebt;

func void Auto_1000_ManaRegen()
{
    if (Npc_IsDead(hero))
    {
        NB_ManaDebt = 0;
        return;
    };

    if (Npc_GetTalentSkill(hero, NPC_TALENT_MAGE) < 1)
    {
        NB_ManaDebt = 0;
        return;
    };

    var int maxMana;
    maxMana = hero.attribute[ATR_MANA_MAX];
    if (maxMana <= 0)
    {
        NB_ManaDebt = 0;
        return;
    };

    if (hero.attribute[ATR_MANA] >= maxMana)
    {
        NB_ManaDebt = 0;
        return;
    };

    var int regen1000;
    var int regen;

    // Mana in thousandths: maxMana * 10 = 1% of max mana.
    regen1000 = maxMana * 10;
    if (regen1000 < 1000)
    {
        regen1000 = 1000;
    };

    NB_ManaDebt += regen1000;
    regen = NB_ManaDebt / 1000;
    NB_ManaDebt -= regen * 1000;

    hero.attribute[ATR_MANA] += regen;
    if (hero.attribute[ATR_MANA] >= maxMana)
    {
        hero.attribute[ATR_MANA] = maxMana;
        NB_ManaDebt = 0;
    };
};
