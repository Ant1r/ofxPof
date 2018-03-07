/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBase.h"
#include "pofWin.h"
#include "pofHead.h"
#include "pofRect.h"
#include "pofBang.h"
#include "pofTranslate.h"
#include "pofRotate.h"
#include "pofScale.h"
#include "pofResetMatrix.h"
#include "pofGetMatrix.h"
#include "pofColor.h"
#include "pofTouch.h"
#include "pofFont.h"
#include "pofText.h"
#include "pofFonts.h"
#include "pofTexts.h"
#include "pofImage.h"
#include "pofFbo.h"
#include "pofTouchable.h"
#include "pofVisible.h"
#include "pofScope.h"
#include "pofCirc.h"
#include "pofUtil.h"
#include "pofXML.h"
#include "pofAccel.h"
#include "pofTextbox.h"
#include "pofJSON.h"
#include "pofSphere.h"
#include "pofPlane.h"
#include "pofDepthTest.h"
#include "pofFilm.h"
#include "pofQuad.h"
#include "pofPath.h"
#include "pofTexture.h"
#include "pofShader.h"
#include "pofFill.h"
#include "pofLayer.h"


void setupAll() {
	pofWin::setup();
	pofHead::setup();
	pofRect::setup();
	pofBang::setup();
	pofTranslate::setup();
	pofRotate::setup();
	pofScale::setup();
	pofResetMatrix::setup();
	pofGetMatrix::setup();
	pofColor::setup();
	pofTouch::setup();
	pofTouchable::setup();
	pofVisible::setup();
	pofFont::setup();
	pofText::setup();
	pofFonts::setup();
	pofTexts::setup();
	pofImage::setup();
	pofFbo::setup();
	pofScope::setup();
	pofCirc::setup();
	pofUtil::setup();
	pofXML::setup();
	pofAccel::setup();
	pofTextbox::setup();
	pofJSON::setup();
	pofPlane::setup();
	pofSphere::setup();
	pofDepthTest::setup();
	pofFilm::setup();
	pofQuad::setup();
	pofPath::setup();
	pofTexture::setup();
	pofShader::setup();
	pofFill::setup();
	pofLayer::setup();
}

