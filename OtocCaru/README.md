# Otoč čáru – doplněk pro Archicad 30 (macOS)

Vlastní doplněk, který obrátí směr vybraných čar (prohodí počáteční a koncový bod).
Příkaz najdete v nabídce **Možnosti** (Options), položka **Otoč čáru**.

## Jak doplněk sestavit (jednorázově, bez programování)

1. Přihlaste se na https://github.com (účet je zdarma) a vytvořte nový repozitář
   (tlačítko **New**, název např. `OtocCaru`).
2. V repozitáři zvolte **Add file → Upload files** a přetáhněte do okna
   **obsah této složky** (soubory `config.json`, `CMakeLists.txt`, `LICENSE.md`
   a složky `Src`, `RINT`, `RFIX`, `RFIX.win`, `Tools`). Potvrďte **Commit changes**.
3. Zvolte **Add file → Create new file**. Do pole s názvem napište přesně
   `.github/workflows/build.yml` a vložte obsah souboru `build.yml`, který jste dostali
   zvlášť. Potvrďte **Commit changes**.
4. Otevřete záložku **Actions**. Sestavení se spustí samo (trvá několik minut).
   Po dokončení (zelená fajfka) otevřete poslední běh a dole v sekci **Artifacts**
   stáhněte `OtocCaru-Archicad30-Mac`.
5. Rozbalte stažený soubor. Uvnitř je ZIP s doplňkem `OtocCaru.bundle`.

## Jak doplněk nainstalovat do Archicadu

1. Složku s `OtocCaru.bundle` uložte na stabilní místo (např. `~/Documents/ArchicadDoplnky`).
2. Pokud macOS doplněk blokuje, otevřete Terminál a spusťte (cestu upravte):
   `xattr -dr com.apple.quarantine ~/Documents/ArchicadDoplnky`
3. V Archicadu 30: **Možnosti → Správce doplňků** (Options → Add-On Manager),
   záložka **Upravit seznam** → **Přidat** a zvolte složku. Restartujte Archicad.

## Jak se doplněk používá

Vyberte jednu nebo více čar a zvolte **Možnosti → Otoč čáru**.
Změna jde vrátit zpět klávesami Cmd+Z.

## Poznámky

- Mění se pouze počáteční a koncový bod čáry. Nastavení šipek na koncích se nemění.
- Doplněk byl napsán podle vývojářské sady Archicad API DevKit 30.3000
  a oficiální CMake šablony Graphisoft (licence MIT, viz `LICENSE.md`).
