# sheeps
2D computer graphics project

Dokumentacija:

Poljana sa ovcama,
Kada se predje desnim klikom (RMB) preko ovce razlete se particles i osisamo je.
Posle nekog vremena (shaven_timeout) joj opet izraste vuna - vazi samo za glavnu ovcu koja se krece na WASD.
Imamo bunar negde random na terenu. Kada kliknemo na njega stvori se ovca.
Da se brze naprave ovce moze klikom dugmeta Q.
Postoje 20% sanse da se stvori negative ovca, pri cemu se desava grmljavina, dok se ovca ne osisa.
Kada se kreira negative ovca, koristi se negative filter u funkciji u rafgl.h draw_spritesheet_negative().
Oluja se sastoji iz 4 faze:
1. Zatamnjenog kruga (po kruznici ima gradijent da vise lici na oblak) koji se siri do radiusa 100,
2. Pauza 2 sekunde,
3. Prosirenje kruga do kraja ekrana,
4. Grmljavina

Grmljavina se sastoji iz flickera, da bi simuliralo bljestanje groma (dva trenutka u kojima se menja brightness).
Struktura groma je napravljena koristeci L systems, gde su tacke izmedju vrha i dna groma rekurzivno izgenerisane,
koristeći random težine (weights). I da bi simuliralo osvetljenje munje, koristi se Gaussian blur (4 okolna pixela).
Kada se negative ovca osisa (desnim klikom), oluja prestaje.

Kada se drzi T (formira 10 kruzica koji rotiraju),
nacrtace se munje izmedju korisnikove i negative ovce.

Kružići koji se iscrtavaju oko ovce se odredjuju koristeći sin(brojac) za y,
i cos(brojac) za x. Brojac se povecava svaki frejm.

KOMANDE:
T - test za hitboxove, crta munju kada se iscrta svih 10 kruzica
RMB - sisaj ovcu,
LMB - na bunar spawnuj ovcu,
Q - spawnovanje bez cooldowna,
LMB hold - moze da se prenese ovca negde (samo glavna korisnicka)


Modified>

rafgl.h:
line 653,686: Spritesheetovi su imali malu liniju na vrhu, (do formata vrv) pa da krene da iscrtava posle nje

sprites skinuti:
https://www.spriters-resource.com/pc_computer/stardewvalley/sheet/169024/
