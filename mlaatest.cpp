#include <irrlicht/irrlicht.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace io;
using namespace scene;

#include "screenquad.h"
#include "input.h"
#include "shadercb.h"
#include "rnd.h"
#include "areamap.h"
#include "helpmsg.h"

#include "mlaa/offsetvs.h"
#include "mlaa/color1fs.h"
#include "mlaa/blend2fs.h"
#include "mlaa/neigh3fs.h"

static void die(const char *msg) {
	fprintf(stderr,"%s\n",msg);
	exit(1);
}

enum state_t {
	MLAA_OFF = 0,
	MLAA_ON
};

int main(int argc, char **argv) {

	// Help?
	if (argv[1] && argv[1][0] == '-') die(helpmsg);

	putenv((char *) "vblank_mode=0"); // No vsync for us, thanks.

	MyEventReceiver *r = new MyEventReceiver();
	IrrlichtDevice *dev = createDevice(EDT_OPENGL, core::dimension2d<u32>(1024,768), 32,
				false, false, false, r);
	if (!dev) die("Can't initialize Irrlicht");

	IVideoDriver *drv = dev->getVideoDriver();
	ISceneManager *smgr = dev->getSceneManager();
	IGPUProgrammingServices *gpu = drv->getGPUProgrammingServices();
	ICameraSceneNode *cam = NULL;
	ITexture *pic = NULL;
	IMeshSceneNode *ball = NULL;
	bool showpic = false;

	IReadFile *areamap = createMemoryReadFile(AreaMap33, sizeof(AreaMap33), "AreaMap33", false);
	if (!areamap) die("Failed to load areamap");
	ITexture *areamaptex = drv->getTexture(areamap);
	areamap->drop();

	// If there's an argument, assume it is a pic to load; otherwise, draw a sphere

	if (argv[1] && access(argv[1], R_OK) == 0) {
		showpic = true;
		pic = drv->getTexture(argv[1]);
		if (!pic) die("Can't load image");

		cam = smgr->addCameraSceneNode();
	} else {
		cam = smgr->addCameraSceneNodeMaya();
		cam->setTarget(vector3df(0, 0, 0));
		ball = smgr->addSphereSceneNode(40, 8);

		int ballshader = gpu->addHighLevelShaderMaterial(rnd,0,EVST_VS_1_1,0);
		ball->setMaterialType((E_MATERIAL_TYPE) ballshader);

		ISceneNodeAnimator *cool = smgr->createRotationAnimator(vector3df(-0.1, 0.1, -0.1));
		ball->addAnimator(cool);
		cool->drop();
	}

	dimension2d<u32> screensize = drv->getScreenSize();
	char defines[128];
	snprintf(defines, 128,
		"#define PIXEL_SIZE vec2(1.0f / %u.0, 1.0f / %u.0)\n"
		"#define MAX_SEARCH_STEPS 8.0\n#define MAX_DISTANCE 33.0\n",
		screensize.Width, screensize.Height);

	ITexture *rt1 = drv->addRenderTargetTexture(screensize, "rt1", ECF_A8R8G8B8);
	ITexture *rt2 = drv->addRenderTargetTexture(screensize, "rt2", ECF_A8R8G8B8);
	ITexture *rt3 = drv->addRenderTargetTexture(screensize, "rt3", ECF_A8R8G8B8);
	if (!rt1 || !rt2 || !rt3) die("No RTT");

	ScreenQuad *def = new ScreenQuad(drv);
	ScreenQuad *sq = new ScreenQuad(drv);
	ScreenQuad *sq2 = new ScreenQuad(drv);
	ScreenQuad *sq3 = new ScreenQuad(drv);
	ScreenQuad *norm = new ScreenQuad(drv);
	if (showpic) def->SetTexture(pic);
	sq->SetTexture(rt1);
	norm->SetTexture(rt1);

	sq2->SetTexture(rt2);
	sq2->SetTexture(rt2, 1);
	sq2->SetTexture(areamaptex, 2);

	sq3->SetTexture(rt3);
	sq3->SetTexture(rt1,1);
	state_t state = MLAA_OFF;

	stringc tmp1, tmp2;
	tmp1 = defines;
	tmp1 += offsetvs;
	tmp2 = defines;
	tmp2 += color1fs;

	int edge = gpu->addHighLevelShaderMaterial(tmp1.c_str(),0,EVST_VS_1_1,tmp2.c_str());
	sq->SetMaterialType((E_MATERIAL_TYPE) edge);

	tmp2 = defines;
	tmp2 += blend2fs;

	blendcb *bcb = new blendcb();
	edge = gpu->addHighLevelShaderMaterial(tmp1.c_str(),0,EVST_VS_1_1,tmp2.c_str(),0,EPST_PS_1_1,bcb);
	sq2->SetMaterialType((E_MATERIAL_TYPE) edge);

	tmp2 = defines;
	tmp2 += neigh3fs;

	neighcb *ncb = new neighcb();
	edge = gpu->addHighLevelShaderMaterial(tmp1.c_str(),0,EVST_VS_1_1,tmp2.c_str(),0,EPST_PS_1_1,ncb);
	sq3->SetMaterialType((E_MATERIAL_TYPE) edge);

	int lastfps = -1, minfps = 10000;
	unsigned long long total_frames = 0;
	struct timeval starttime;
	gettimeofday(&starttime, NULL);
	wchar_t cap[20];

	while (dev->run()) {
		drv->beginScene();

		switch (state) {
			case MLAA_OFF:
				if (showpic) def->Render(false);
				else smgr->drawAll();
			break;
			case MLAA_ON:
				if (showpic) def->Render(rt1);
				else {
					drv->setRenderTarget(rt1);
					smgr->drawAll();
				}
				sq->Render(rt2);
				sq2->Render(rt3);

				// Draw the initial image to the framebuffer, smoothing will be overlaid
				norm->Render();
				sq3->Render(false);
			break;
		}

		drv->endScene();

		int fps = drv->getFPS();
		if (minfps > fps) minfps = fps;
		if (lastfps != fps) {
			swprintf(cap, 20, L"%d fps, MLAA %s", fps, state == MLAA_ON ? "on" : "off");
			dev->setWindowCaption(cap);
			lastfps = fps;
		}

		if (r->IsKeyDown(KEY_KEY_M)) {
			if (state == MLAA_ON) state = MLAA_OFF;
			else state = MLAA_ON;

			lastfps++;
		}

		usleep(1); // 16?
		total_frames++;
	}

	dev->drop();
	delete ncb;
	delete bcb;
	delete def;
	delete sq;
	delete r;

	struct timeval endtime;
	gettimeofday(&endtime, NULL);
	float sec = endtime.tv_sec - starttime.tv_sec;
	sec += ((float) endtime.tv_usec - starttime.tv_usec) / 1000000;

	printf("Ran %.3fs, average fps %.2f, min %d\n", sec, (float) total_frames/sec, minfps);

	return 0;
}
