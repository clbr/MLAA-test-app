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
	NONE = 0,
	R,
	RG,
	RGB
};

int main(int argc, char **argv) {

	putenv((char *)"vblank_mode=0");

	MyEventReceiver *r = new MyEventReceiver();
	IrrlichtDevice *dev = createDevice(EDT_OPENGL, core::dimension2d<u32>(1024,768), 32,
				false, false, false, r);
	if (!dev) die("Can't initialize Irrlicht");


	IVideoDriver *drv = dev->getVideoDriver();
	ISceneManager *smgr = dev->getSceneManager();

	ICameraSceneNode *cam = smgr->addCameraSceneNode();
	cam->setPosition(vector3df(0, 0, -30));
	cam->setTarget(vector3df(0,0,0));
//	smgr->setAmbientLight(SColor(0,60,60,60));

//	ITexture *pic = drv->getTexture("skies.jpg");
//	ITexture *pic = drv->getTexture("rainbow.png");
	ITexture *rt1 = drv->addRenderTargetTexture(dimension2d<u32>(1024,768), "rt1");
	ITexture *rt2 = drv->addRenderTargetTexture(dimension2d<u32>(1024,768), "rt2");
	ITexture *def = drv->addRenderTargetTexture(dimension2d<u32>(1024,768), "def");

	ScreenQuad *defsq = new ScreenQuad(drv);
	ScreenQuad *sqr = new ScreenQuad(drv);
	ScreenQuad *sqrg = new ScreenQuad(drv);
	ScreenQuad *sqrgb = new ScreenQuad(drv);
	defsq->SetTexture(pic);
	sqr->SetTexture(def);
	sqrg->SetTexture(rt1);
	sqrgb->SetTexture(rt2);
	int state = NONE;

	IGPUProgrammingServices *gpu = drv->getGPUProgrammingServices();
	int nogreens = gpu->addHighLevelShaderMaterial(0,0,EVST_VS_1_1,nogreen);
	int noreds = gpu->addHighLevelShaderMaterial(0,0,EVST_VS_1_1,nored);
	int noblues = gpu->addHighLevelShaderMaterial(0,0,EVST_VS_1_1,noblue);
	sqr->SetMaterialType((E_MATERIAL_TYPE) noreds);
	sqrg->SetMaterialType((E_MATERIAL_TYPE) nogreens);
	sqrgb->SetMaterialType((E_MATERIAL_TYPE) noblues);

	int lastfps = -1;
	wchar_t cap[10];

	while (dev->run()) { //if (dev->isWindowActive()) {
		drv->beginScene();

		switch (state) {
			case NONE:
				defsq->Render();
			break;
			case R:
				defsq->Render(def);
				sqr->Render();
			break;
			case RG:
				defsq->Render(def);
				sqr->Render(rt1);
				sqrg->Render();
			break;
			case RGB:
				defsq->Render(def);
				sqr->Render(rt1);
				sqrg->Render(rt2);
				sqrgb->Render();
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
			state++;
			if (state > RGB) state = NONE;
			printf("%d\n",state);
		}

		usleep(1); // 16
	} //else usleep(50);

	dev->drop();
	delete sqr;
	delete sqrg;
	delete sqrgb;
	delete r;
	return 0;
}
