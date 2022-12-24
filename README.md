# Unreal-Minimal-Viable-Flecs
Minimal viable Flecs entity component system implementation in Unreal Engine.

Intended as a super simple starting point / reference for using Flecs in Unreal.


* Flecs v3.1.2 & Unreal Engine 5.1
* As simple as it can be
* It does a thing
* ISM based entity rendering
* Uses the Flecs c++ API
* [Flecs Explorer](https://github.com/flecs-hub/explorer) integration

Huge thans to [Red J](https://github.com/jtferson) for his [flecs space battle tutorial](https://jtferson.github.io/blog/quickstart_with_flecs_in_unreal_part_1/), check it out if you want a deeper dive
### Demo


### Why

Sometimes you need an ECS to make heavy stuff work performance-wise, Unreal doesn't have one built-in.

[Mass](https://docs.unrealengine.com/5.1/en-US/mass-entity-in-unreal-engine/) is still experimental (but if you want to use it [check out this](https://github.com/Megafunk/MassSample))

Flecs is a well built performant solution for doing ECS used by [ConfettiFX's The Forge](https://github.com/ConfettiFX/The-Forge#release-153---october-5th-2022---steamdeck-support--app-life-cycle-changes--shader-byte-code-offline-generation--gtao-unit-test--improved-gradient-calculation-in-visibility-buffer--new-c-containers--reorg-tf-directory-structure--upgraded-to-newer-imgui--the-forge-blog) who are powering Bethesda's next-gen game Starfield and a bunch of other projects.

Flecs has [great documentation](https://www.flecs.dev/flecs/#/?id=getting-started) and an active community where you can ask your questions
