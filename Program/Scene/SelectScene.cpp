#include "SelectScene.h"

SelectScene::SelectScene() {

}

SelectScene::~SelectScene()
{
}

void SelectScene::OnEnter()
{
}

void SelectScene::OnExit()
{
}

void SelectScene::Update()
{
}

void SelectScene::Draw()
{
}

void SelectScene::ApplyGlobalVariables() {
	/*GlobalVariables* global = GlobalVariables::GetInstance();
	const char* groupName = "UIRandom";

	angleMinMax_.first = global->Get<int>(groupName, "AngleRandomMin");
	angleMinMax_.second = global->Get<int>(groupName, "AngleRandomMax");
	posMinMax_.first = global->Get<Vector2>(groupName, "PosRandomMin");
	posMinMax_.second = global->Get<Vector2>(groupName, "PosRandomMax");*/
}

void SelectScene::TextureSetting() {
	/*backGround_->sprite->SetPosition(backGround_->originalTransform.translate_);
	backGround_->sprite->SetScale(backGround_->originalTransform.scale_);
	backGround_->sprite->SetPivot({ 0.5f,0.5f });

	texDetas_ = TextureEditor::GetInstance()->GetTitleTextures();

	randAngle_ = SoLib::Random::GetRandom(angleMinMax_.first, angleMinMax_.second);
	randPos_ = SoLib::Random::GetRandom(posMinMax_.first, posMinMax_.second);

	for (size_t i = 0; i < texDetas_.size(); i++) {
		Tex2DState* nowTex = texDetas_[i];
		if (nowTex->textureName == "TitleStartUI") {
			nowTex->angle_degrees = randAngle_;
			nowTex->transform.rotate_ = DegreeToRadian(nowTex->angle_degrees);
			nowTex->transform.translate_ = nowTex->originalTransform.translate_ + randPos_;
		}
	}*/


}