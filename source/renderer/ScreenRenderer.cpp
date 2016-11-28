#include <radix/renderer/ScreenRenderer.hpp>
#include <radix/Viewport.hpp>
#include <epoxy/gl.h>
#include <radix/model/MeshLoader.hpp>
#include <radix/shader/ShaderLoader.hpp>
#include <radix/text/FontLoader.hpp>

namespace radix {

ScreenRenderer::ScreenRenderer(World &w, Renderer &ren, GameWorld &gw) :
  SubRenderer(w, ren),
  gameWorld(gw) { }

void ScreenRenderer::render() {
  for(Screen *screen : *gameWorld.getScreens()) {
    renderScreen(screen);
  }
}

void ScreenRenderer::renderScreen(Screen *screen) {
  glDepthMask(GL_FALSE);

  renderer.getViewport()->getSize(&viewportWidth, &viewportHeight);

  initCamera();

  Matrix4f widget;
  int xAxisViewportCenter = viewportWidth / 2;
  int yAxisViewportCenter = viewportHeight / 2;
  widget.translate(Vector3f(xAxisViewportCenter, yAxisViewportCenter, -18));
  widget.scale(Vector3f(viewportWidth, viewportHeight, 1));

  const Mesh &mesh = MeshLoader::getMesh("GUIElement.obj");
  Shader &sh = ShaderLoader::getShader("color.frag");

  sh.bind();
  glUniform4f(sh.uni("color"), screen->color.r, screen->color.g, screen->color.b, screen->color.a);
  renderer.renderMesh(*renderContext, sh, widget, mesh);

  sh.release();

  for (unsigned int i = 0; i < screen->text.size(); i++) { // render text
    screen->text[i].font = "Pacaya";
    Font font = FontLoader::getFont(screen->text[i].font);
    font.size = screen->text[i].size;
    int textWidth = font.getStringLength(screen->text[i].content);
    Vector3f position(0, 0, screen->text[i].z);

    position.y = viewportHeight - screen->text[i].top;
    Text::Align textAlign = screen->text[i].align;
    if (textAlign == Text::Center) {
      position.x = xAxisViewportCenter - (textWidth / 2);
    } else if (textAlign == Text::Left) {
      position.x = (xAxisViewportCenter - viewportWidth / 4) - (textWidth / 2);
    } else if (textAlign == Text::Right) {
      position.x = (xAxisViewportCenter + viewportWidth / 4) - (textWidth / 2);
    }

    screen->text[i].position = position;

    renderer.renderText(*renderContext, screen->text[i]);
  }

  glDepthMask(GL_TRUE);
}

} /* namespace radix */
