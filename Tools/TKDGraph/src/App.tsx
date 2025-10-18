import './App.css';
import { NodeEditor } from './components';
import { EditorContext } from './context';
import { NodeRegistry } from './utils';
import { useState, useEffect } from 'react';
import type { BlueprintData } from './types';

const STORAGE_KEY = 'tkd-graph-blueprints';
const STORAGE_INDEX_KEY = 'tkd-graph-current-index';

function App() {
  const nodeRegistry = new NodeRegistry();
  const classList: string[] = [];

  // Load blueprints from localStorage on initial mount
  const [blueprints, setBlueprints] = useState<BlueprintData[]>(() => {
    try {
      const stored = localStorage.getItem(STORAGE_KEY);
      if (stored) {
        const parsed = JSON.parse(stored);
        console.log('Loaded blueprints from localStorage:', parsed.length);
        return parsed;
      }
    } catch (error) {
      console.error('Failed to load blueprints from localStorage:', error);
    }
    return [];
  });

  const [currentBlueprintIndex, setCurrentBlueprintIndex] = useState<number>(() => {
    try {
      const stored = localStorage.getItem(STORAGE_INDEX_KEY);
      if (stored) {
        return parseInt(stored, 10);
      }
    } catch (error) {
      console.error('Failed to load current index from localStorage:', error);
    }
    return -1;
  });

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

  // Auto-save blueprints to localStorage whenever they change
  useEffect(() => {
    try {
      if (blueprints.length > 0) {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(blueprints));
        console.log('Auto-saved blueprints to localStorage:', blueprints.length);
      } else {
        // Clear storage if no blueprints
        localStorage.removeItem(STORAGE_KEY);
      }
    } catch (error) {
      console.error('Failed to save blueprints to localStorage:', error);
    }
  }, [blueprints]);

  // Auto-save current blueprint index
  useEffect(() => {
    try {
      if (currentBlueprintIndex >= 0) {
        localStorage.setItem(STORAGE_INDEX_KEY, currentBlueprintIndex.toString());
      } else {
        localStorage.removeItem(STORAGE_INDEX_KEY);
      }
    } catch (error) {
      console.error('Failed to save current index to localStorage:', error);
    }
  }, [currentBlueprintIndex]);

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
