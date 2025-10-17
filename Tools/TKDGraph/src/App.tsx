import './App.css';
import { NodeEditor } from './components';
import { EditorContext } from './context';
import { NodeRegistry } from './utils';
import { useState } from 'react';
import type { BlueprintData } from './types';

function App() {
  const nodeRegistry = new NodeRegistry();
  const classList: string[] = [];
  const [blueprints, setBlueprints] = useState<BlueprintData[]>([]);
  const [currentBlueprintIndex, setCurrentBlueprintIndex] = useState<number>(-1);

  const addBlueprint = (blueprint: BlueprintData) => {
    setBlueprints((prev) => {
      const newBlueprints = [...prev, blueprint];
      setCurrentBlueprintIndex(newBlueprints.length - 1);
      return newBlueprints;
    });
  };

  const removeBlueprint = (index: number) => {
    setBlueprints((prev) => prev.filter((_, i) => i !== index));
    setCurrentBlueprintIndex(-1);
  };

  const contextValue = {
    nodeRegistry,
    classList,
    blueprints,
    currentBlueprintIndex,
    setBlueprints,
    setCurrentBlueprintIndex,
    addBlueprint,
    removeBlueprint
  };

  return (
    <>
      <EditorContext.Provider value={contextValue}>
        <NodeEditor />
      </EditorContext.Provider>
    </>
  );
}

export default App;
