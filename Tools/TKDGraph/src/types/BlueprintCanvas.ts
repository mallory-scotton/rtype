/**
 * @brief CanvasTransform Type
 * @description Represents the transformation state of the blueprint canvas, including translation and scaling factors.
 */
export type CanvasTransform = {
  translateX: number;
  translateY: number;
  scale: number;
};

/**
 * @brief MultiSelectBox Type
 * @description Represents the coordinates of a multi-selection box on the canvas.
 */
export type MultiSelectBox = {
  startX: number;
  startY: number;
  currentX: number;
  currentY: number;
};
