#include <irrlicht/irrlicht.h>
#include <stdlib.h>
#include <unistd.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

#include "screenquad.h"
#include "input.h"

static void die(const char *msg) {
	fprintf(stderr,"%s\n",msg);
	exit(1);
}

enum state_t {
	MLAA_OFF = 0,
	MLAA_ON
};

int main(int argc, char **argv) {

	putenv((char *) "vblank_mode=0"); // No vsync for us, thanks.

	MyEventReceiver *r = new MyEventReceiver();
	IrrlichtDevice *dev = createDevice(EDT_OPENGL, core::dimension2d<u32>(1024,768), 32,
				false, false, false, r);
	if (!dev) die("Can't initialize Irrlicht");

	IVideoDriver *drv = dev->getVideoDriver();
	ISceneManager *smgr = dev->getSceneManager();
	ITexture *pic = NULL;
	bool showpic = false;

	if (argv[1] && access(argv[1], R_OK) == 0) {
		showpic = true;
		pic = drv->getTexture(argv[1]);
	}

	ICameraSceneNode *cam = smgr->addCameraSceneNode();
	cam->setPosition(vector3df(0, 0, -30));
	cam->setTarget(vector3df(0,0,0));

	ITexture *rt1 = drv->addRenderTargetTexture(dimension2d<u32>(1024,768), "rt1");

	ScreenQuad *def = new ScreenQuad(drv);
	ScreenQuad *sq = new ScreenQuad(drv);
	if (showpic) def->SetTexture(pic);
	sq->SetTexture(rt1);
	state_t state = MLAA_OFF;

//	IGPUProgrammingServices *gpu = drv->getGPUProgrammingServices();
//	int nogreens = gpu->addHighLevelShaderMaterial(0,0,EVST_VS_1_1,nogreen);
//	sq->SetMaterialType((E_MATERIAL_TYPE) noreds);

	int lastfps = -1;
	wchar_t cap[10];

	while (dev->run()) {
		drv->beginScene();

		if (!showpic) smgr->drawAll();

		switch (state) {
			case MLAA_OFF:
				def->Render();
			break;
			case MLAA_ON:
				def->Render(rt1);
				sq->Render();
			break;
		}

		drv->endScene();

		int fps = drv->getFPS();
		if (lastfps != fps) {
			swprintf(cap, 10, L"%d fps", fps);
			dev->setWindowCaption(cap);
			lastfps = fps;
		}

		if (r->IsKeyDown(KEY_KEY_M)) {
			if (state == MLAA_ON) state = MLAA_OFF;
			else state = MLAA_ON;
		}

		usleep(1); // 16
	}

	dev->drop();
	delete def;
	delete sq;
	delete r;
	return 0;
}
