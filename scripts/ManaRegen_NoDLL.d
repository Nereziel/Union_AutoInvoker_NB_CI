META
{
    Parser = Game;
    MergeMode = 1;
    Engine = G2A;
};

// Gothic II: New Balance - mana regeneration without AutoInvoker DLL.
// Requires zParserExtender integrated into Union.
// 1% max mana per second (min. 1); only learned magic circles.
// No full-HP requirement. Regenerates while in combat.

var int NB_ManaDebt;
var int NB_ManaCheckFrames;

func void NB_ManaRegen_Apply()
{
    var int maxMana;
    var int regen1000;
    var int regen;

    if (Npc_IsDead(hero))
    {
        NB_ManaDebt = 0;
        return;
    };

    // Do not regenerate mana for characters without magic circles.
    if (Npc_GetTalentSkill(hero, NPC_TALENT_MAGE) < 1)
    {
        NB_ManaDebt = 0;
        return;
    };

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

    // Thousandths allow fractional regeneration, e.g. 1.5 mana/s.
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

// The zParserExtender trigger needs an int return value.
func int NB_ManaRegen_Tick()
{
    NB_ManaRegen_Apply();
    return LOOP_CONTINUE;
};

// GameLoop is used only to ensure the timer exists after loading saves.
// It does not regenerate mana per frame.
func event GameLoop()
{
    var C_Trigger existingTrigger;

    NB_ManaCheckFrames += 1;
    if (NB_ManaCheckFrames < 30)
    {
        return;
    };
    NB_ManaCheckFrames = 0;

    // Trigger state is persisted by zParserExtender; avoid duplicates.
    existingTrigger = AI_GetNextTriggerByFuncName(null, "NB_ManaRegen_Tick");
    if (Hlp_IsNull(existingTrigger))
    {
        existingTrigger = AI_StartTriggerScript("NB_ManaRegen_Tick", 1000);
    };
};
