/**
* @file mat4.cpp
* @author Repertoi-e (Repertoire, dsotirov@windowslive.com)
* @version 1.0
*
* @section Info
* Taken from: https://github.com/TheCherno/Sparky/tree/master/Sparky-core/src/sp/maths

* @section LICENSE
*
*      						  Apache License
*                        Version 2.0, January 2004
*                     http://www.apache.org/licenses/
* 
* TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
* 
* 1. Definitions.
* 
*   "License" shall mean the terms and conditions for use, reproduction,
*   and distribution as defined by Sections 1 through 9 of this document.
* 
*   "Licensor" shall mean the copyright owner or entity authorized by
*   the copyright owner that is granting the License.
* 
*   "Legal Entity" shall mean the union of the acting entity and all
*   other entities that control, are controlled by, or are under common
*   control with that entity. For the purposes of this definition,
*   "control" means (i) the power, direct or indirect, to cause the
*   direction or management of such entity, whether by contract or
*   otherwise, or (ii) ownership of fifty percent (50%) or more of the
*   outstanding shares, or (iii) beneficial ownership of such entity.
* 
*   "You" (or "Your") shall mean an individual or Legal Entity
*   exercising permissions granted by this License.
* 
*   "Source" form shall mean the preferred form for making modifications,
*   including but not limited to software source code, documentation
*   source, and configuration files.
* 
*   "Object" form shall mean any form resulting from mechanical
*   transformation or translation of a Source form, including but
*   not limited to compiled object code, generated documentation,
*   and conversions to other media types.
* 
*   "Work" shall mean the work of authorship, whether in Source or
*   Object form, made available under the License, as indicated by a
*   copyright notice that is included in or attached to the work
*   (an example is provided in the Appendix below).
* 
*   "Derivative Works" shall mean any work, whether in Source or Object
*   form, that is based on (or derived from) the Work and for which the
*   editorial revisions, annotations, elaborations, or other modifications
*   represent, as a whole, an original work of authorship. For the purposes
*   of this License, Derivative Works shall not include works that remain
*   separable from, or merely link (or bind by name) to the interfaces of,
*   the Work and Derivative Works thereof.
* 
*   "Contribution" shall mean any work of authorship, including
*   the original version of the Work and any modifications or additions
*   to that Work or Derivative Works thereof, that is intentionally
*   submitted to Licensor for inclusion in the Work by the copyright owner
*   or by an individual or Legal Entity authorized to submit on behalf of
*   the copyright owner. For the purposes of this definition, "submitted"
*   means any form of electronic, verbal, or written communication sent
*   to the Licensor or its representatives, including but not limited to
*   communication on electronic mailing lists, source code control systems,
*   and issue tracking systems that are managed by, or on behalf of, the
*   Licensor for the purpose of discussing and improving the Work, but
*   excluding communication that is conspicuously marked or otherwise
*   designated in writing by the copyright owner as "Not a Contribution."
* 
*   "Contributor" shall mean Licensor and any individual or Legal Entity
*   on behalf of whom a Contribution has been received by Licensor and
*   subsequently incorporated within the Work.
* 
* 2. Grant of Copyright License. Subject to the terms and conditions of
*   this License, each Contributor hereby grants to You a perpetual,
*   worldwide, non-exclusive, no-charge, royalty-free, irrevocable
*   copyright license to reproduce, prepare Derivative Works of,
*   publicly display, publicly perform, sublicense, and distribute the
*   Work and such Derivative Works in Source or Object form.
* 
* 3. Grant of Patent License. Subject to the terms and conditions of
*   this License, each Contributor hereby grants to You a perpetual,
*   worldwide, non-exclusive, no-charge, royalty-free, irrevocable
*   (except as stated in this section) patent license to make, have made,
*   use, offer to sell, sell, import, and otherwise transfer the Work,
*   where such license applies only to those patent claims licensable
*   by such Contributor that are necessarily infringed by their
*   Contribution(s) alone or by combination of their Contribution(s)
*   with the Work to which such Contribution(s) was submitted. If You
*   institute patent litigation against any entity (including a
*   cross-claim or counterclaim in a lawsuit) alleging that the Work
*   or a Contribution incorporated within the Work constitutes direct
*   or contributory patent infringement, then any patent licenses
*   granted to You under this License for that Work shall terminate
*   as of the date such litigation is filed.
* 
* 4. Redistribution. You may reproduce and distribute copies of the
*   Work or Derivative Works thereof in any medium, with or without
*   modifications, and in Source or Object form, provided that You
*   meet the following conditions:
* 
*   (a) You must give any other recipients of the Work or
*       Derivative Works a copy of this License; and
* 
*   (b) You must cause any modified files to carry prominent notices
*       stating that You changed the files; and
* 
*   (c) You must retain, in the Source form of any Derivative Works
*       that You distribute, all copyright, patent, trademark, and
*       attribution notices from the Source form of the Work,
*       excluding those notices that do not pertain to any part of
*       the Derivative Works; and
* 
*   (d) If the Work includes a "NOTICE" text file as part of its
*       distribution, then any Derivative Works that You distribute must
*       include a readable copy of the attribution notices contained
*       within such NOTICE file, excluding those notices that do not
*       pertain to any part of the Derivative Works, in at least one
*       of the following places: within a NOTICE text file distributed
*       as part of the Derivative Works; within the Source form or
*       documentation, if provided along with the Derivative Works; or,
*       within a display generated by the Derivative Works, if and
*       wherever such third-party notices normally appear. The contents
*       of the NOTICE file are for informational purposes only and
*       do not modify the License. You may add Your own attribution
*       notices within Derivative Works that You distribute, alongside
*       or as an addendum to the NOTICE text from the Work, provided
*       that such additional attribution notices cannot be construed
*       as modifying the License.
* 
*   You may add Your own copyright statement to Your modifications and
*   may provide additional or different license terms and conditions
*   for use, reproduction, or distribution of Your modifications, or
*   for any such Derivative Works as a whole, provided Your use,
*   reproduction, and distribution of the Work otherwise complies with
*   the conditions stated in this License.
* 
* 5. Submission of Contributions. Unless You explicitly state otherwise,
*   any Contribution intentionally submitted for inclusion in the Work
*   by You to the Licensor shall be under the terms and conditions of
*   this License, without any additional terms or conditions.
*   Notwithstanding the above, nothing herein shall supersede or modify
*   the terms of any separate license agreement you may have executed
*   with Licensor regarding such Contributions.
* 
* 6. Trademarks. This License does not grant permission to use the trade
*   names, trademarks, service marks, or product names of the Licensor,
*   except as required for reasonable and customary use in describing the
*   origin of the Work and reproducing the content of the NOTICE file.
* 
* 7. Disclaimer of Warranty. Unless required by applicable law or
*   agreed to in writing, Licensor provides the Work (and each
*   Contributor provides its Contributions) on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
*   implied, including, without limitation, any warranties or conditions
*   of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
*   PARTICULAR PURPOSE. You are solely responsible for determining the
*   appropriateness of using or redistributing the Work and assume any
*   risks associated with Your exercise of permissions under this License.
* 
* 8. Limitation of Liability. In no event and under no legal theory,
*   whether in tort (including negligence), contract, or otherwise,
*   unless required by applicable law (such as deliberate and grossly
*   negligent acts) or agreed to in writing, shall any Contributor be
*   liable to You for damages, including any direct, indirect, special,
*   incidental, or consequential damages of any character arising as a
*   result of this License or out of the use or inability to use the
*   Work (including but not limited to damages for loss of goodwill,
*   work stoppage, computer failure or malfunction, or any and all
*   other commercial damages or losses), even if such Contributor
*   has been advised of the possibility of such damages.
* 
* 9. Accepting Warranty or Additional Liability. While redistributing
*   the Work or Derivative Works thereof, You may choose to offer,
*   and charge a fee for, acceptance of support, warranty, indemnity,
*   or other liability obligations and/or rights consistent with this
*   License. However, in accepting such obligations, You may act only
*   on Your own behalf and on Your sole responsibility, not on behalf
*   of any other Contributor, and only if You agree to indemnify,
*   defend, and hold each Contributor harmless for any liability
*   incurred by, or claims asserted against, such Contributor by reason
*   of your accepting any such warranty or additional liability.
* 
* END OF TERMS AND CONDITIONS
* 
* APPENDIX: How to apply the Apache License to your work.
* 
*   To apply the Apache License to your work, attach the following
*   boilerplate notice, with the fields enclosed by brackets "{}"
*   replaced with your own identifying information. (Don't include
*   the brackets!)  The text should be enclosed in the appropriate
*   comment syntax for the file format. We also recommend that a
*   file or class name and description of purpose be included on the
*   same "printed page" as the copyright notice for easier
*   identification within third-party archives.
* 
* Copyright {yyyy} {name of copyright owner}
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* 
*    http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "cl/stdafx.h"
#include "mat4.h"

#include "maths_func.h"

namespace cl { 

	mat4::mat4()
	{
		memset(elements, 0, 4 * 4 * sizeof(float32));
	}

	mat4::mat4(float32 diagonal)
	{
		memset(elements, 0, 4 * 4 * sizeof(float32));
		elements[0 + 0 * 4] = diagonal;
		elements[1 + 1 * 4] = diagonal;
		elements[2 + 2 * 4] = diagonal;
		elements[3 + 3 * 4] = diagonal;
	}

	mat4::mat4(float32* elements)
	{
		memcpy(this->elements, elements, 4 * 4 * sizeof(float32));
	}

	mat4::mat4(const vec4& row0, const vec4& row1, const vec4& row2, const vec4& row3)
	{
		rows[0] = row0;
		rows[1] = row1;
		rows[2] = row2;
		rows[3] = row3;
	}

	mat4 mat4::Identity()
	{
		return mat4(1.f);
	}

	mat4& mat4::Multiply(const mat4& other)
	{
		float32 data[16];
		for (s32 row = 0; row < 4; row++)
			for (s32 col = 0; col < 4; col++)
			{
				float32 sum = 0.0f;
				for (s32 e = 0; e < 4; e++)
					sum += elements[e + row * 4] * other.elements[col + e * 4];
				data[col + row * 4] = sum;
			}
		memcpy(elements, data, 4 * 4 * sizeof(float32));
		return *this;
	}

	vec3 mat4::Multiply(const vec3& other) const
	{
		return other.Multiply(*this);
	}

	vec4 mat4::Multiply(const vec4& other) const
	{
		return other.Multiply(*this);
	}

	mat4 operator*(mat4 left, const mat4& right)
	{
		return left.Multiply(right);
	}

	mat4& mat4::operator*=(const mat4& other)
	{
		return Multiply(other);
	}

	vec3 operator*(const mat4& left, const vec3& right)
	{
		return left.Multiply(right);
	}

	vec4 operator*(const mat4& left, const vec4& right)
	{
		return left.Multiply(right);
	}

	mat4& mat4::Invert()
	{
		float32 temp[16];

		temp[0] = elements[5] * elements[10] * elements[15] -
			elements[5] * elements[11] * elements[14] -
			elements[9] * elements[6] * elements[15] +
			elements[9] * elements[7] * elements[14] +
			elements[13] * elements[6] * elements[11] -
			elements[13] * elements[7] * elements[10];

		temp[4] = -elements[4] * elements[10] * elements[15] +
			elements[4] * elements[11] * elements[14] +
			elements[8] * elements[6] * elements[15] -
			elements[8] * elements[7] * elements[14] -
			elements[12] * elements[6] * elements[11] +
			elements[12] * elements[7] * elements[10];

		temp[8] = elements[4] * elements[9] * elements[15] -
			elements[4] * elements[11] * elements[13] -
			elements[8] * elements[5] * elements[15] +
			elements[8] * elements[7] * elements[13] +
			elements[12] * elements[5] * elements[11] -
			elements[12] * elements[7] * elements[9];

		temp[12] = -elements[4] * elements[9] * elements[14] +
			elements[4] * elements[10] * elements[13] +
			elements[8] * elements[5] * elements[14] -
			elements[8] * elements[6] * elements[13] -
			elements[12] * elements[5] * elements[10] +
			elements[12] * elements[6] * elements[9];

		temp[1] = -elements[1] * elements[10] * elements[15] +
			elements[1] * elements[11] * elements[14] +
			elements[9] * elements[2] * elements[15] -
			elements[9] * elements[3] * elements[14] -
			elements[13] * elements[2] * elements[11] +
			elements[13] * elements[3] * elements[10];

		temp[5] = elements[0] * elements[10] * elements[15] -
			elements[0] * elements[11] * elements[14] -
			elements[8] * elements[2] * elements[15] +
			elements[8] * elements[3] * elements[14] +
			elements[12] * elements[2] * elements[11] -
			elements[12] * elements[3] * elements[10];

		temp[9] = -elements[0] * elements[9] * elements[15] +
			elements[0] * elements[11] * elements[13] +
			elements[8] * elements[1] * elements[15] -
			elements[8] * elements[3] * elements[13] -
			elements[12] * elements[1] * elements[11] +
			elements[12] * elements[3] * elements[9];

		temp[13] = elements[0] * elements[9] * elements[14] -
			elements[0] * elements[10] * elements[13] -
			elements[8] * elements[1] * elements[14] +
			elements[8] * elements[2] * elements[13] +
			elements[12] * elements[1] * elements[10] -
			elements[12] * elements[2] * elements[9];

		temp[2] = elements[1] * elements[6] * elements[15] -
			elements[1] * elements[7] * elements[14] -
			elements[5] * elements[2] * elements[15] +
			elements[5] * elements[3] * elements[14] +
			elements[13] * elements[2] * elements[7] -
			elements[13] * elements[3] * elements[6];

		temp[6] = -elements[0] * elements[6] * elements[15] +
			elements[0] * elements[7] * elements[14] +
			elements[4] * elements[2] * elements[15] -
			elements[4] * elements[3] * elements[14] -
			elements[12] * elements[2] * elements[7] +
			elements[12] * elements[3] * elements[6];

		temp[10] = elements[0] * elements[5] * elements[15] -
			elements[0] * elements[7] * elements[13] -
			elements[4] * elements[1] * elements[15] +
			elements[4] * elements[3] * elements[13] +
			elements[12] * elements[1] * elements[7] -
			elements[12] * elements[3] * elements[5];

		temp[14] = -elements[0] * elements[5] * elements[14] +
			elements[0] * elements[6] * elements[13] +
			elements[4] * elements[1] * elements[14] -
			elements[4] * elements[2] * elements[13] -
			elements[12] * elements[1] * elements[6] +
			elements[12] * elements[2] * elements[5];

		temp[3] = -elements[1] * elements[6] * elements[11] +
			elements[1] * elements[7] * elements[10] +
			elements[5] * elements[2] * elements[11] -
			elements[5] * elements[3] * elements[10] -
			elements[9] * elements[2] * elements[7] +
			elements[9] * elements[3] * elements[6];

		temp[7] = elements[0] * elements[6] * elements[11] -
			elements[0] * elements[7] * elements[10] -
			elements[4] * elements[2] * elements[11] +
			elements[4] * elements[3] * elements[10] +
			elements[8] * elements[2] * elements[7] -
			elements[8] * elements[3] * elements[6];

		temp[11] = -elements[0] * elements[5] * elements[11] +
			elements[0] * elements[7] * elements[9] +
			elements[4] * elements[1] * elements[11] -
			elements[4] * elements[3] * elements[9] -
			elements[8] * elements[1] * elements[7] +
			elements[8] * elements[3] * elements[5];

		temp[15] = elements[0] * elements[5] * elements[10] -
			elements[0] * elements[6] * elements[9] -
			elements[4] * elements[1] * elements[10] +
			elements[4] * elements[2] * elements[9] +
			elements[8] * elements[1] * elements[6] -
			elements[8] * elements[2] * elements[5];

		float32 determinant = elements[0] * temp[0] + elements[1] * temp[4] + elements[2] * temp[8] + elements[3] * temp[12];
		determinant = 1.f / determinant;

		for (s32 i = 0; i < 4 * 4; i++)
			elements[i] = temp[i] * determinant;

		return *this;
	}

	vec4 mat4::GetColumn(int index) const
	{
		return vec4(elements[index + 0 * 4], elements[index + 1 * 4], elements[index + 2 * 4], elements[index + 3 * 4]);
	}

	void mat4::SetColumn(u32 index, const vec4& column)
	{
		elements[index + 0 * 4] = column.x;
		elements[index + 1 * 4] = column.y;
		elements[index + 2 * 4] = column.z;
		elements[index + 3 * 4] = column.w;
	}

	mat4 mat4::Orthographic(float32 left, float32 right, float32 bottom, float32 top, float32 near, float32 far)
	{
		mat4 result = Identity();

		result.elements[0 + 0 * 4] = 2.f / (right - left);

		result.elements[1 + 1 * 4] = 2.f / (top - bottom);

		result.elements[2 + 2 * 4] = 2.f / (near - far);

		result.elements[3 + 0 * 4] = (left + right) / (left - right);
		result.elements[3 + 1 * 4] = (bottom + top) / (bottom - top);
		result.elements[3 + 2 * 4] = (far + near) / (far - near);

		return result;
	}

	mat4 mat4::Perspective(float32 fov, float32 aspectRatio, float32 near, float32 far)
	{
		mat4 result = Identity();

		float32 q = 1.f / tan(toRadians(0.5f * fov));
		float32 a = q / aspectRatio;

		float32 b = (near + far) / (near - far);
		float32 c = (2.f * near * far) / (near - far);

		result.elements[0 + 0 * 4] = a;
		result.elements[1 + 1 * 4] = q;
		result.elements[2 + 2 * 4] = b;
		result.elements[2 + 3 * 4] = -1.f;
		result.elements[3 + 2 * 4] = c;

		return result;
	}

	mat4 mat4::LookAt(const vec3& camera, const vec3& object, const vec3& up)
	{
		mat4 result = Identity();
		vec3 f = (object - camera).Normalize();
		vec3 s = f.Cross(up.Normalize());
		vec3 u = s.Cross(f);

		result.elements[0 + 0 * 4] = s.x;
		result.elements[0 + 1 * 4] = s.y;
		result.elements[0 + 2 * 4] = s.z;

		result.elements[1 + 0 * 4] = u.x;
		result.elements[1 + 1 * 4] = u.y;
		result.elements[1 + 2 * 4] = u.z;

		result.elements[2 + 0 * 4] = -f.x;
		result.elements[2 + 1 * 4] = -f.y;
		result.elements[2 + 2 * 4] = -f.z;

		return result * Translate(vec3(-camera.x, -camera.y, -camera.z));
	}

	mat4 mat4::Translate(const vec3& translation)
	{
		mat4 result = Identity();

		result.elements[3 + 0 * 4] = translation.x;
		result.elements[3 + 1 * 4] = translation.y;
		result.elements[3 + 2 * 4] = translation.z;

		return result;
	}

	mat4 mat4::Rotate(float32 angle, const vec3& axis)
	{
		mat4 result = Identity();

		float32 r = toRadians(angle);
		float32 c = cos(r);
		float32 s = sin(r);
		float32 omc = 1.f - c;

		float32 x = axis.x;
		float32 y = axis.y;
		float32 z = axis.z;

		result.elements[0 + 0 * 4] = x * x * omc + c;
		result.elements[0 + 1 * 4] = y * x * omc + z * s;
		result.elements[0 + 2 * 4] = x * z * omc - y * s;

		result.elements[1 + 0 * 4] = x * y * omc - z * s;
		result.elements[1 + 1 * 4] = y * y * omc + c;
		result.elements[1 + 2 * 4] = y * z * omc + x * s;

		result.elements[2 + 0 * 4] = x * z * omc + y * s;
		result.elements[2 + 1 * 4] = y * z * omc - x * s;
		result.elements[2 + 2 * 4] = z * z * omc + c;

		return result;
	}

	mat4 mat4::Scale(const vec3& scale)
	{
		mat4 result = Identity();

		result.elements[0 + 0 * 4] = scale.x;
		result.elements[1 + 1 * 4] = scale.y;
		result.elements[2 + 2 * 4] = scale.z;

		return result;
	}

	mat4 mat4::Invert(const mat4& matrix)
	{
		mat4 result = matrix;
		return result.Invert();
	}

	mat4 mat4::Transpose(const mat4& matrix)
	{
		return mat4(
			vec4(matrix.rows[0].x, matrix.rows[1].x, matrix.rows[2].x, matrix.rows[3].x),
			vec4(matrix.rows[0].y, matrix.rows[1].y, matrix.rows[2].y, matrix.rows[3].y),
			vec4(matrix.rows[0].z, matrix.rows[1].z, matrix.rows[2].z, matrix.rows[3].z),
			vec4(matrix.rows[0].w, matrix.rows[1].w, matrix.rows[2].w, matrix.rows[3].w)
		);
	}
}
