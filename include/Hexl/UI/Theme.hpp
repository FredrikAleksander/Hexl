#pragma once
#ifndef __HEXL_UI__THEME_HPP
#define __HEXL_UI__THEME_HPP 1

#include <Hexl/String.hpp>
#include <map>
#include <functional>

namespace Hexl {
namespace UI {

class StyleEditorWindow;
class Theme {
	public:
		using FlagChangeHandler = std::function<void(const Hexl::String&, bool)>;
		using ColorChangeHandler = std::function<void(const Hexl::String&, float, float, float, float)>;
		using ScalarChangeHandler = std::function<void(const Hexl::String&, float)>;
		using Vec2ChangeHandler = std::function<void(const Hexl::String&, float, float)>;
		using Vec3ChangeHandler = std::function<void(const Hexl::String&, float, float, float)>;
		using Vec4ChangeHandler = std::function<void(const Hexl::String&, float, float, float, float)>;
	private:
		friend class StyleEditorWindow;
		struct ThemeInfo {
			Hexl::String description;
		};
		struct ThemeFlag {
			ThemeInfo info;
			bool      value;
			bool      defined;
			std::vector<FlagChangeHandler> changeHandlers;
		};
		struct ThemeColor {
			ThemeInfo info;
			float     value[4];
			bool      defined;
			std::vector<ColorChangeHandler> changeHandlers;
		};
		struct ThemeScalar {
			ThemeInfo info;
			float     value;
			bool      defined;
			std::vector<ScalarChangeHandler> changeHandlers;
		};
		struct ThemeVec2 {
			ThemeInfo info;
			float     value[2];
			bool      defined;
			std::vector<Vec2ChangeHandler> changeHandlers;
		};
		struct ThemeVec3 {
			ThemeInfo info;
			float     value[3];
			bool      defined;
			std::vector<Vec3ChangeHandler> changeHandlers;
		};
		struct ThemeVec4 {
			ThemeInfo info;
			float     value[4];
			bool      defined;
			std::vector<Vec4ChangeHandler> changeHandlers;
		};

		using FlagMap = std::map<Hexl::String, ThemeFlag>;
		using ColorMap = std::map<Hexl::String, ThemeColor>;
		using ScalarMap = std::map<Hexl::String, ThemeScalar>;
		using Vec2Map = std::map<Hexl::String, ThemeVec2>;
		using Vec3Map = std::map<Hexl::String, ThemeVec3>;
		using Vec4Map = std::map<Hexl::String, ThemeVec4>;

		size_t    m_change;

		FlagMap   m_flags;
		ColorMap  m_colors;
		ScalarMap m_scalars;
		Vec2Map   m_vec2s;
		Vec3Map   m_vec3s;
		Vec4Map   m_vec4s;

		ThemeFlag& GetOrCreateFlag(const Hexl::String& name);
		ThemeColor& GetOrCreateColor(const Hexl::String& name);
		ThemeScalar& GetOrCreateScalar(const Hexl::String& name);
		ThemeVec2& GetOrCreateVec2(const Hexl::String& name);
		ThemeVec3& GetOrCreateVec3(const Hexl::String& name);
		ThemeVec4& GetOrCreateVec4(const Hexl::String& name);
	public:
		Theme();
		Hexl::String ExportCurrentSettings();
		size_t GetNumChanges() const;


		bool IsFlagDescribed(const Hexl::String& name) const;
		bool IsFlagDefined(const Hexl::String& name) const;
		bool IsColorDescribed(const Hexl::String& name) const;
		bool IsColorDefined(const Hexl::String& name) const;
		bool IsScalarDescribed(const Hexl::String& name) const;
		bool IsScalarDefined(const Hexl::String& name) const;
		bool IsVec2Described(const Hexl::String& name) const;
		bool IsVec2Defined(const Hexl::String& name) const;
		bool IsVec3Described(const Hexl::String& name) const;
		bool IsVec3Defined(const Hexl::String& name) const;
		bool IsVec4Described(const Hexl::String& name) const;
		bool IsVec4Defined(const Hexl::String& name) const;

		void DescribeFlag(const Hexl::String& name, Hexl::String&& description);

		/*!
		 * Give the specified color a description
		 */
		void DescribeColor(const Hexl::String& name, Hexl::String&& description);

		/*!
		 * Give the specified scalar a description
		 */
		void DescribeScalar(const Hexl::String& name, Hexl::String&& description);

		void DescribeVec2(const Hexl::String& name, Hexl::String&& description);

		void DescribeVec3(const Hexl::String& name, Hexl::String&& description);

		void DescribeVec4(const Hexl::String& name, Hexl::String&& description);

		void BindFlag(const Hexl::String& name, FlagChangeHandler&& handler, bool immediate = true);

		/*!
		 * Bind a change handler for a color. This is used to be notified of a color change
		 */
		void BindColor(const Hexl::String& name, ColorChangeHandler&& handler, bool immediate = true);

		/*!
		 * Bind a change handler for a scalar. This is used to be notified of a color change
		 */
		void BindScalar(const Hexl::String& name, ScalarChangeHandler&& handler, bool immediate = true);

		void BindVec2(const Hexl::String& name, Vec2ChangeHandler&& handler, bool immediate = true);

		void BindVec3(const Hexl::String& name, Vec3ChangeHandler&& handler, bool immediate = true);

		void BindVec4(const Hexl::String& name, Vec4ChangeHandler&& handler, bool immediate = true);

		void SetFlag(const Hexl::String& name, bool value);

		/*!
		 * Set a color to a specified value
		 */
		void SetColor(const Hexl::String& name, float r, float g, float b, float a);

		/*!
		 * Set a color to a specified value
		 */
		void SetScalar(const Hexl::String& name, float value);

		void SetVec2(const Hexl::String& name, float x, float y);

		void SetVec3(const Hexl::String& name, float x, float y, float z);

		void SetVec4(const Hexl::String& name, float x, float y, float z, float w);
};

}
}

#endif
