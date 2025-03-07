# CTRL.StateTree

This plugin adds a range of useful new state tree utility tasks and property functions.

## Pawn State Tree Component

Provides a state tree component and schema for use with `APawn` and `AController` classes, enabling the use of state trees
with *player* controllers and pawns.

Can be configured to require the controller or pawn, or not, in the case where the state tree
will run on another actor, before the controller or pawn is set e.g. on an interactive object.

Call `SetController` to set a controller (and pawn) to be used at a later time.

## Tasks

### GAS

#### Trigger Gameplay Event

Trigger a GameplayEvent on target actor(s), via `AbilitySystemComponent`. Fire on a single actor, or an array of actors,
both of which can be bound to data.
Allows using normal state tree transitions with Gameplay Events e.g. on `Actor.Died` event transition to `Dead` state.
Can fire different (or no) events on Enter and Exit state.

#### Gameplay Event to StateTree Event

Any GameplayEvents received on the target actor, matching the specified tag, will be re-emitted as StateTree events. Allows using normal state tree transitions to respond to Gameplay Events e.g. on `Actor.Died` event transition to `Dead` state.
Useful as a global task, bound to the tree owner. Combined with "Trigger GameplayEvent" this allows bidirectional communication between the state tree and actors.

Options for `bOnlyTriggerOnce` and `bOnlyMatchExact`, like the `WaitGameplayEvent` Blueprint node.

### Input

#### Change Input Config

Change input config (e.g. Game Only, Game & UI, UI Only) & associated options (e.g. mouse capture, cursor, input flush) on target player controller. Reverts on exit.
Changes are debounced and applied on next tick to prevent flickering.

Uses a stack of input configs, so when you exit a state, the next config in the stack will be used.
You may want to set a default config in your player controller's root state to prevent flickering or unexpected behavior.

### UI

#### ActivateWidget

Activates/deactivates a target `UCommonActivatableWidget`. Optionally performs the inverse on exit.

#### Create Widget

Creates a widget of the specified `UUserWidget` class and optionally adds it to the target player's
viewport on state enter/exit.

The widget "template" can be configured as an instanced object, so any exposed properties can be set inline in the state tree editor.

The created widget is made available to the state tree as an output, so you can use it in other tasks. Will provide the widget as a
common activatable widget if it is a subclass of `UCommonActivatableWidget` so you can use it in the "Activate Widget"
task, etc.

### Actors

#### Spawn Actor

Spawns an actor of the specified `UClass` at the specified location, rotation and collision handling method.
Spawned actor is made available to the state tree as an output, so you can use it in other tasks.
Spawned actor can have a configurable random yaw.

Optionally sets the owner and instigator of the spawned actor.
The spawned actor can be optionally destroyed on state exit.

#### Get Owner Actor

Provides the owner actor of the target `UObject` as an output.
If called on an actor, it will return the owner actor.
If called on a component, it will return the owner actor of that component.
Otherwise, finds the first `AActor` outer.

#### Set Actor Collision

Sets the collision settings of the target actor.

#### Set Character Gravity Scale

Sets the gravity scale of the target character. This is useful for zero-gravity effects, or to disable gravity on a character.
Reverts to previous gravity scale on exit by default.

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
git submodule add git@github.com:ntystudio/CTRL.StateTree.git Plugins/CTRLStateTree
git commit -m "Added CTRL.StateTree Plugin as a submodule."
```

Alternatively you can download the ZIP of this repo and place it in `YourProject/Plugins/`.

## License

MIT

----

> [!Tip] 
> # NTY Studio Plugins
>
> * [� Unicode Browser](https://github.com/ntystudio/UnicodeBrowser)
> * [CTRL.StateTree](https://github.com/ntystudio/CTRL.StateTree)
> * [CTRL.ReferenceVisualizer](https://github.com/ntystudio/CTRL.ReferenceVisualizer)
> * [CTRL.Core](https://github.com/ntystudio/CTRL.Core)
> * [CTRL.Gas](https://github.com/ntystudio/CTRL.Gas)
