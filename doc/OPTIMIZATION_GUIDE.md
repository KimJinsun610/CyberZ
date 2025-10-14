# CyberZ 성능 최적화 가이드

## ✅ 자동으로 적용된 최적화

1. Shader DEBUG 플래그 수정

## 🎯 다음 단계 (수동 적용)

### 1. Scene.cpp에 Frustum Culling 적용

```cpp
// Scene::Render 함수 수정
void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {
    for (int i = 0; i < m_nObjects; i++) {
        // ✅ Frustum Culling 적용
        if (m_ppObjects[i]->IsVisible(pCamera)) {
            m_ppObjects[i]->Render(pd3dCommandList, pCamera);
        }
    }
}
```

### 2. Visual Studio 리빌드

1. Visual Studio에서 프로젝트 열기
2. 상단에서 **Release** 모드로 변경
3. 솔루션 탐색기 → CyberZ_Client 우클릭 → **다시 빌드**

### 3. 성능 측정

- 패치 전 FPS: _____
- 패치 후 FPS: _____
- 예상 향상률: **+35-55%**

## 📊 예상 성능 개선

| 항목 | 개선 효과 |
|------|-----------|
| Shader 최적화 | FPS +35% |
| Upload 버퍼 해제 | 메모리 -150MB |
| Frustum Culling | FPS +15-20% |

## 🔄 롤백 방법

백업 위치: `.backup/[timestamp]`

```bash
# 자동 롤백
node auto_fix_v2.js rollback [timestamp]

# 수동 롤백
# .backup 폴더에서 파일 복사
```

---
**자동 생성 일시:** 2025. 10. 13. 오후 6:34:57
