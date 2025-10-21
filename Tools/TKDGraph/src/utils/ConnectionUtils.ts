/**
 * @brief Connection utilities
 * @description Utilities for drawing and managing connections between pins
 */

/**
 * Get the color for a connection based on pin type
 */
export function getLinkColor(element: HTMLElement): string {
  const colorMap: Array<{ css: string; color: string }> = [
    { css: 'asset', color: '95FFFF' },
    { css: 'asset-class', color: 'FF95FF' },
    { css: 'bool', color: '950000' },
    { css: 'byte', color: '006F65' },
    { css: 'class', color: '5900BC' },
    { css: 'default', color: 'E1CCAA' },
    { css: 'delegate', color: 'FF3838' },
    { css: 'exec', color: 'FFFFFF' },
    { css: 'float', color: 'A1FF45' },
    { css: 'index', color: '1EE4AF' },
    { css: 'int', color: '1EE4AF' },
    { css: 'interface', color: 'F1FFAA' },
    { css: 'name', color: 'CD82FF' },
    { css: 'object', color: '00AAF5' },
    { css: 'rotator', color: 'A1B4FF' },
    { css: 'string', color: 'FF00D5' },
    { css: 'struct', color: '0059CC' },
    { css: 'text', color: 'E87CAA' },
    { css: 'transform', color: 'FF7300' },
    { css: 'vector', color: 'FFCA22' },
    { css: 'wildcard', color: '817A7A' },
    { css: 'audio', color: 'FD94FD' },
    { css: 'time', color: '95FEFE' },
    { css: 'wavetable', color: 'C800EB' },
    { css: 'real', color: '38D500' },
    { css: 'pcg-spatial-data', color: 'FFFFFF' },
    { css: 'pcg-attribute-set', color: 'C8811C' },
    { css: 'pcg-point-data', color: '3F89FF' },
    { css: 'pcg-poly-line-data', color: '3FE1EA' },
    { css: 'pcg-landscape-data', color: 'D4D44B' },
    { css: 'pcg-texture-data', color: 'E65019' },
    { css: 'pcg-render-target-data', color: 'E77661' },
    { css: 'pcg-surface-data', color: '45C47E' },
    { css: 'pcg-volume-data', color: 'E645BC' },
    { css: 'pcg-primitive-data', color: '813FFF' },
    { css: 'pcg-concrete-data', color: 'B3A6FA' },
    { css: 'pcg-any-data', color: '939393' }
  ];

  for (const item of colorMap) {
    if (element.classList.contains(item.css)) {
      return item.color;
    }
  }

  return 'E1CCAA'; // default color
}

/**
 * Get correction width for pin element
 */
export function correctionWidth(element: HTMLElement): number {
  return element.classList.contains('connector-image') ? 5 : 6;
}

/**
 * Get correction height for pin element
 */
export function correctionHeight(element: HTMLElement): number {
  return element.classList.contains('connector-image') ? 7 : 6;
}

/**
 * Generate SVG path for connection
 */
export function getSvgPath(
  start: { left: number; top: number },
  end: { left: number; top: number },
  width: number,
  height: number
): string {
  const halfWidth = (width / 2) >> 0;

  if (start.left <= end.left) {
    if (start.top <= end.top) {
      return `M 0 0 C${halfWidth},0 ${halfWidth},${height} ${width},${height}`;
    } else {
      return `M 0 ${height} C${halfWidth},${height} ${halfWidth},0 ${width},0`;
    }
  } else {
    if (start.top <= end.top) {
      return `M ${width} 0 C${width + width},0 ${-width},${height} 0,${height}`;
    } else {
      return `M ${width} ${height} C${width + width},${height} ${-width},0 0,0`;
    }
  }
}

/**
 * Correct SVG path for knot nodes
 */
export function correctSvgPathForKnot(
  path: string,
  startElement: HTMLElement,
  endElement: HTMLElement,
  start: { left: number; top: number },
  end: { left: number; top: number },
  width: number,
  height: number
): string {
  const startIsKnot = startElement.closest('.node')?.classList.contains('knot') ?? false;
  const endIsKnot = endElement.closest('.node')?.classList.contains('knot') ?? false;

  if (!startIsKnot && endIsKnot) {
    const startIsLeftCol = startElement.closest('.left-col') !== null;
    if (!startIsLeftCol && start.left > end.left) {
      if (start.top > end.top) {
        return `M ${width} ${height} C${width + 150},${height} ${width},0 0,0`;
      } else {
        return `M ${width} 0 C${width + 150},0 ${width},${height} 0,${height}`;
      }
    }
  } else if (startIsKnot && !endIsKnot) {
    const endIsLeftCol = endElement.closest('.left-col') !== null;
    if (endIsLeftCol && start.left > end.left) {
      if (end.top > start.top) {
        return `M 0 ${height} C-75,${height} -75,0 ${width},0`;
      } else {
        return `M 0 0 C-75,0 -75,${height} ${width},${height}`;
      }
    }
  }

  return path;
}

/**
 * Compute connection render data
 * The SVG is rendered inside the transformed canvas, so we use the element's
 * offsetLeft/offsetTop which are already in canvas space
 */
export function computeConnectionRenderData(
  startElement: HTMLElement,
  endElement: HTMLElement,
  scale: number
): {
  path: string;
  color: string;
  x: number;
  y: number;
  width: number;
  height: number;
} | null {
  if (!startElement || !endElement) return null;

  // Get the node elements that contain the pins
  const startNode = startElement.closest('.node') as HTMLElement;
  const endNode = endElement.closest('.node') as HTMLElement;

  if (!startNode || !endNode) return null;

  // Get node positions from their transform
  const startTransform = startNode.style.transform;
  const endTransform = endNode.style.transform;

  const startMatch = startTransform.match(/translate\((-?\d+(?:\.\d+)?)px,\s*(-?\d+(?:\.\d+)?)px\)/);
  const endMatch = endTransform.match(/translate\((-?\d+(?:\.\d+)?)px,\s*(-?\d+(?:\.\d+)?)px\)/);

  if (!startMatch || !endMatch) return null;

  const startNodeX = parseFloat(startMatch[1]);
  const startNodeY = parseFloat(startMatch[2]);
  const endNodeX = parseFloat(endMatch[1]);
  const endNodeY = parseFloat(endMatch[2]);

  // Get pin offset within its node
  const startPinRect = startElement.getBoundingClientRect();
  const endPinRect = endElement.getBoundingClientRect();
  const startNodeRect = startNode.getBoundingClientRect();
  const endNodeRect = endNode.getBoundingClientRect();

  const startPinOffsetX = (startPinRect.left - startNodeRect.left + startPinRect.width / 2) / scale;
  const startPinOffsetY = (startPinRect.top - startNodeRect.top + startPinRect.height / 2) / scale;
  const endPinOffsetX = (endPinRect.left - endNodeRect.left + endPinRect.width / 2) / scale;
  const endPinOffsetY = (endPinRect.top - endNodeRect.top + endPinRect.height / 2) / scale;

  // Calculate absolute pin positions in canvas space
  const start = {
    left: startNodeX + startPinOffsetX,
    top: startNodeY + startPinOffsetY
  };

  const end = {
    left: endNodeX + endPinOffsetX,
    top: endNodeY + endPinOffsetY
  };

  // Calculate SVG bounding box position (top-left corner)
  const x = Math.min(start.left, end.left);
  const y = Math.min(start.top, end.top);

  let width = Math.max(start.left, end.left) - Math.min(start.left, end.left);
  let height = Math.max(start.top, end.top) - Math.min(start.top, end.top);

  if (width === 0) width = 2;
  if (height === 0) height = 2;

  // Convert absolute positions to local coordinates within the SVG
  const localStart = {
    left: start.left - x,
    top: start.top - y
  };

  const localEnd = {
    left: end.left - x,
    top: end.top - y
  };

  let path = getSvgPath(localStart, localEnd, width, height);
  path = correctSvgPathForKnot(path, startElement, endElement, localStart, localEnd, width, height);

  const color = getLinkColor(startElement);

  return { path, color, x, y, width, height };
}

/**
 * Compute connection render data with mouse position (for temporary connection while dragging)
 * Need to convert mouse position from screen space to canvas space
 */
export function computeTempConnectionRenderData(
  startElement: HTMLElement,
  mousePos: { x: number; y: number },
  canvasTransform: { translateX: number; translateY: number; scale: number },
  startDirection: 'input' | 'output'
): {
  path: string;
  color: string;
  x: number;
  y: number;
  width: number;
  height: number;
} | null {
  if (!startElement) return null;

  // Get the node element that contains the pin
  const startNode = startElement.closest('.node') as HTMLElement;
  if (!startNode) return null;

  // Get node position from its transform
  const startTransform = startNode.style.transform;
  const startMatch = startTransform.match(/translate\((-?\d+(?:\.\d+)?)px,\s*(-?\d+(?:\.\d+)?)px\)/);
  if (!startMatch) return null;

  const startNodeX = parseFloat(startMatch[1]);
  const startNodeY = parseFloat(startMatch[2]);

  // Get pin offset within its node
  const startPinRect = startElement.getBoundingClientRect();
  const startNodeRect = startNode.getBoundingClientRect();

  const startPinOffsetX = (startPinRect.left - startNodeRect.left + startPinRect.width / 2) / canvasTransform.scale;
  const startPinOffsetY = (startPinRect.top - startNodeRect.top + startPinRect.height / 2) / canvasTransform.scale;

  // Calculate absolute pin position in canvas space
  const pinPosition = {
    left: startNodeX + startPinOffsetX,
    top: startNodeY + startPinOffsetY
  };

  // Convert mouse position from screen space to canvas space
  // Get the canvas element to find its screen position
  const canvas = startNode.closest('.canvas') as HTMLElement;
  if (!canvas) return null;

  const canvasRect = canvas.getBoundingClientRect();

  const mousePosition = {
    left: (mousePos.x - canvasRect.left) / canvasTransform.scale,
    top: (mousePos.y - canvasRect.top) / canvasTransform.scale
  };

  // If starting from an input pin, swap start and end so the curve goes backwards (right to left)
  const start = startDirection === 'input' ? mousePosition : pinPosition;
  const end = startDirection === 'input' ? pinPosition : mousePosition;

  // Calculate SVG bounding box position (top-left corner)
  const x = Math.min(start.left, end.left);
  const y = Math.min(start.top, end.top);

  let width = Math.max(start.left, end.left) - Math.min(start.left, end.left);
  let height = Math.max(start.top, end.top) - Math.min(start.top, end.top);

  if (width === 0) width = 2;
  if (height === 0) height = 2;

  // Convert absolute positions to local coordinates within the SVG
  const localStart = {
    left: start.left - x,
    top: start.top - y
  };

  const localEnd = {
    left: end.left - x,
    top: end.top - y
  };

  const path = getSvgPath(localStart, localEnd, width, height);
  const color = getLinkColor(startElement);

  return { path, color, x, y, width, height };
}

/**
 * Snap value to grid
 */
export function snapToGrid(value: number, gridSize: number = 16): number {
  return Math.round(value / gridSize) * gridSize;
}
