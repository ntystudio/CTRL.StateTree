# Use State Tree

This plugin adds a range of useful new state tree utility tasks and property functions.

## Tasks

### Events

#### Trigger Gameplay Event

Trigger a GameplayEvent on target actor(s), via `AbilitySystemComponent`. Fire on a single actor, or an array of actors, both of which can be bound to data.
Can fire different (or no) events on Enter and Exit state.

#### Forward Gameplay Event to StateTree Event

Any GameplayEvents received on the target actor, matching the specified tag, will be re-emitted as StateTree events. Allows using normal state tree transitions to respond to Gameplay Events e.g. on `Actor.Died` event transition to `Dead` state.
Useful as a global task, bound to the tree owner. Combined with "Trigger GameplayEvent" this allows bidirectional communication between the state tree and actors.
Options for `bOnlyTriggerOnce` and `bOnlyMatchExact`, like the `WaitGameplayEvent` Blueprint node.

### Input

#### Change Input Config

Change input config (e.g. Game Only, Game & UI, UI Only) & associated options (e.g. mouse capture, cursor, input flush) on target player controller. Reverts on exit.
Changes are debounced and applied on next tick to prevent flickering.
Uses a stack of configs, so when you exit a state, the next config in the stack will be used.
You may want to put a default config in your root state to prevent flickering.

### UI

#### ActivateWidget

Activates/deactivates a target `UCommonActivatableWidget`. Optionally performs the inverse on exit.

### Components

#### Set Component Visibility

Sets the visibility of the target component to the target value. Unsets the visibility on exit.

#### Set Component Visibility In-Game

Same as "Set Component Visibility" but calls the appropriate `SetHiddenInGame` on the target component.

### Debug

#### Print Text

Log a message to the output console and/or screen on enter/exit state. Same general options as regular `Print Text` Blueprint Node.

## Property Functions

### String to Text
Useful for binding text properties to string data.

### Bool To String
Useful for logging.

* Random Float
* Random Integer
* Object Is Valid

## Supported Engine Versions

The plugin was developed for Unreal Engine 5.5.1+. It is unlikely to work in versions of Unreal Engine <5.5.0 due to heavy changes in the StateTree plugin. Please let me know if you get it working outside 5.5.1+.

## Installation

The best way is to clone this repository as a submodule; that way you can contribute
pull requests and more importantly, easily get the latest updates.
 
The project should be placed in your project's `Plugins` folder.

```bash
cd YourProject
git submodule add git@github.com:ntystudio/UseStateTree.git Plugins/UseStateTree
git commit -m "Added UseStateTree Plugin as a submodule."
```

Alternatively you can download the ZIP of this repo and place it in `YourProject/Plugins/`.

## License

MIT

----

----

> [!Tip] 
> # NTY Studio Plugins
>
> * [� Unicode Browser](https://github.com/ntystudio/UnicodeBrowser)
> * [Use State Tree](https://github.com/ntystudio/UseStateTree)
> * [Reference Visualizer](https://github.com/ntystudio/CTRL-reference-visualizer)
