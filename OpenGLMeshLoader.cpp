if (showSt) {
		glPushMatrix();
		if (showT1) {
			glTranslated(0, 1.55, 0);
		}
		else {
			glTranslated(0, moveDown, 0);

		}

		glScaled(0.0006, 0.0006, 0.0006);
		glRotated(Angle, 0, 1, 0);
		model_diamond.Draw();
		glPopMatrix();
	}