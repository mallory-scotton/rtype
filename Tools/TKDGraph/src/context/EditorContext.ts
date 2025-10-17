/** Dependencies */
import { NodeRegistry } from '../utils';
import { createContext } from 'react';
import type { BlueprintData } from '../types';

/**
 * @brief Editor context react hook
 * @returns Editor context value
 */
export const EditorContext = createContext<{
  nodeRegistry: NodeRegistry;
  classList: string[];
  blueprints: BlueprintData[];
  currentBlueprintIndex: number;
  setBlueprints: (blueprints: BlueprintData[]) => void;
  setCurrentBlueprintIndex: (index: number) => void;
  addBlueprint: (blueprint: BlueprintData) => void;
  removeBlueprint: (index: number) => void;
}>({
  nodeRegistry: new NodeRegistry(),
  classList: [],
  blueprints: [],
  currentBlueprintIndex: -1,
  setCurrentBlueprintIndex: () => {},
  setBlueprints: () => {},
  addBlueprint: () => {},
  removeBlueprint: () => {}
});
